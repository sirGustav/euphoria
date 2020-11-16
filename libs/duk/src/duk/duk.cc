#include "duk/duk.h"

#include <sstream>

#include "duktape/duktape.h"

#include "core/assert.h"
#include "core/str.h"
#include "core/stringmerger.h"

#include "duk/function.h"
#include "duk/bindobject.h"
#include "duk/bindclass.h"

namespace euphoria::duk
{
    Duk::Duk()
        : Context(
                duk_create_heap(
                        nullptr,
                        nullptr,
                        nullptr,
                        nullptr,
                        FatalHandler),
                this)
        , references(ctx)
    {}

    ObjectReference
    Duk::CreateGlobal(const std::string& name)
    {
        duk_push_object(ctx);
        ObjectReference val = ObjectReference {duk_get_heapptr(ctx, -1)};
        duk_put_global_string(ctx, name.c_str());
        return val;
    }

    Context*
    Duk::AsContext()
    {
        return this;
    }

    bool
    Duk::EvalString(
            const std::string& line,
            const std::string& file,
            std::string*       error,
            std::string*       output)
    {
        duk_push_string(ctx, line.c_str());
        duk_push_string(ctx, file.c_str());

        const auto compile_result = duk_pcompile(ctx, 0);
        bool       ok             = false;
        if(compile_result != 0)
        {
            if(error)
            {
                *error = duk_safe_to_string(ctx, -1);
            }
            ok = false;
        }
        else
        {
            const auto call_result = duk_pcall(ctx, 0);
            if(call_result != DUK_EXEC_SUCCESS)
            {
                if(error)
                {
                    CollectDukError(ctx, error);
                }
                ok = false;
            }
            else
            {
                if(output)
                {
                    *output = VarToString(
                            ctx, -1);  // duk_safe_to_string(ctx, -1);
                }
                ok = true;
            }
        }
        duk_pop(ctx);

        return ok;
    }


    int
    DukPrintFunctionCallback(duk_context* ctx)
    {
        if(duk_is_constructor_call(ctx))
        {
            return duk_type_error(ctx, "%s", "Not a constructor call");
        }

        duk_push_current_function(ctx);
        duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("duk"));
        auto* duk = reinterpret_cast<Duk*>(duk_to_pointer(ctx, -1));
        duk_pop(ctx);  // duk pointer
        duk_pop(ctx);  // current function

        const int number_of_arguments = duk_get_top(ctx);

        std::stringstream ss;

        auto first = true;

        for(int arg = number_of_arguments; arg > 0; arg -= 1)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                ss << " ";
            }
            ss << VarToString(ctx, -arg);
        }

        duk->on_print(ss.str());

        return 0;
    }

    void
    Duk::BindPrint(std::function<void(const std::string&)> new_on_print)
    {
        this->on_print = new_on_print;

        duk_push_c_function(ctx, DukPrintFunctionCallback, DUK_VARARGS);  // fun
        duk_push_pointer(ctx, this);  // fun pointer
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("duk"));  // fun

        const auto function_added = duk_put_global_string(ctx, "print");
        ASSERTX(function_added == 1, function_added);
    }

    std::string
    GetDukType(Context* a, int index)
    {
        duk_context* ctx = a->ctx;
        if(duk_is_number(ctx, index))
        {
            return "number";
        }
        if(duk_is_object(ctx, index))
        {
            return "object";
        }
        if(duk_is_string(ctx, index))
        {
            return "string";
        }
        return "???";
    }

    std::string
    DescribeArguments(Context* ctx)
    {
        const int                args = ctx->GetNumberOfArguments();
        std::vector<std::string> types;
        for(int i = 0; i < args; i += 1)
        {
            const int index = -args + i;
#if 1
            const std::string err = core::Str() << GetDukType(ctx, index) << "="
                                                << VarToString(ctx->ctx, index);
            types.emplace_back(err);
#else
            types.emplace_back(GetDukType(ctx, index));
#endif
        }
        return core::StringMerger::FunctionCall().Generate(types);
    }

    template <typename T>
    T*
    GetFunctionProperty(duk_context* ctx, const char* name)
    {
        void* ptr = GetVoidFunctionProperty(ctx, name);
        return reinterpret_cast<T*>(ptr);
    }

    template <bool TPushThis, bool TConstructor>
    int
    DukGenericFunctionCallback(duk_context* ctx)
    {
        if(TConstructor)
        {
            if(!duk_is_constructor_call(ctx))
            {
                if(TPushThis)
                {
                    return duk_type_error(ctx, "%s", "Not a method call");
                }
                else
                {
                    return duk_type_error(ctx, "%s", "Not a function call");
                }
            }
        }
        else
        {
            if(duk_is_constructor_call(ctx))
            {
                return duk_type_error(ctx, "%s", "Not a constructor call");
            }
        }

        auto* function
                = GetFunctionProperty<Function>(ctx, DUK_HIDDEN_SYMBOL("func"));
        auto* duk = GetFunctionProperty<Duk>(ctx, DUK_HIDDEN_SYMBOL("duk"));

        Context context {ctx, duk};

        if(TPushThis)
        {
            const auto arguments = duk_get_top(ctx);
            duk_push_this(ctx);
            if(arguments != 0)
            {
                // don't insert if there are 0 arguments
                duk_insert(ctx, -(arguments + 1));
            }
        }

        ASSERT(function);

        {
            const auto match_error = function->Matches(&context);
            if(match_error.empty())
            {
                return function->Call(&context);
            }
            else
            {
                std::string described = core::Str()
                                        << "- " << function->Describe(&context)
                                        << ": " << match_error;

                const auto arguments = DescribeArguments(&context);
                return duk_type_error(
                        ctx,
                        "No matches found for %s, tried %s",
                        arguments.c_str(),
                        described.c_str());
            }
        }
    }

    void
    PlaceFunctionOnStack(
            duk_context*   ctx,
            Function*      function,
            duk_c_function fun,
            Duk*           duk,
            int            arguments = DUK_VARARGS)
    {
        duk_push_c_function(ctx, fun, arguments);  // fun

        duk_push_pointer(ctx, function);  // fun pointer
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("func"));  // fun

        duk_push_pointer(ctx, duk);  // fun pointer
        duk_put_prop_string(ctx, -2, DUK_HIDDEN_SYMBOL("duk"));  // fun
    }

    void
    Duk::BindGlobalFunction(
            const std::string&               name,
            const std::shared_ptr<Function>& bind)
    {
        PlaceFunctionOnStack(
                ctx,
                CreateFunction(bind),
                DukGenericFunctionCallback<false, false>,
                this);

        const auto function_added = duk_put_global_string(ctx, name.c_str());
        ASSERTX(function_added == 1, function_added);
    }

    void
    Duk::BindObject(const std::string& name, const ObjectBinder& bind)
    {
        const auto object_index = duk_push_object(ctx);

        for(const auto& func: bind.functions)
        {
            PlaceFunctionOnStack(
                    ctx,
                    CreateFunction(func.function),
                    DukGenericFunctionCallback<false, false>,
                    this);

            const auto function_added
                    = duk_put_prop_string(ctx, object_index, func.name.c_str());
            ASSERTX(function_added == 1, function_added);
        }

        const auto object_added = duk_put_global_string(ctx, name.c_str());
        ASSERTX(object_added == 1, object_added);
    }

    void
    Duk::BindClass(const std::string& name, const ClassBinder& bind)
    {
        if(bind.constructor)
        {
            // constructor
            PlaceFunctionOnStack(
                    ctx,
                    CreateFunction(bind.constructor),
                    DukGenericFunctionCallback<false, true>,
                    this);  // constructor
            const auto constructor_added
                    = duk_put_global_string(ctx, name.c_str());
            ASSERTX(constructor_added == 1, constructor_added);
        }

        // use duk_push_bare_object?
        const auto prototype_index = duk_push_object(ctx);  // prototype

        for(const auto& method: bind.methods)
        {
            PlaceFunctionOnStack(
                    ctx,
                    CreateFunction(method.function),
                    DukGenericFunctionCallback<true, false>,
                    this);  // proto func
            const auto function_added = duk_put_prop_string(
                    ctx, prototype_index, method.name.c_str());  // proto
            ASSERTX(function_added == 1, function_added);
        }

        for(const auto& prop: bind.properties)
        {
            duk_uint_t flags = 0;

            duk_push_string(ctx, prop.name.c_str());

            if(prop.get)
            {
                PlaceFunctionOnStack(
                        ctx,
                        CreateFunction(prop.get),
                        DukGenericFunctionCallback<true, false>,
                        this,
                        0);
                flags |= DUK_DEFPROP_HAVE_GETTER;
            }
            if(prop.set)
            {
                PlaceFunctionOnStack(
                        ctx,
                        CreateFunction(prop.set),
                        DukGenericFunctionCallback<true, false>,
                        this,
                        1);
                flags |= DUK_DEFPROP_HAVE_SETTER;
            }
            // a getter, setter or both must be set
            ASSERTX(flags != 0, flags);
            duk_def_prop(ctx, prototype_index, flags);
        }

        void*             prototype  = duk_get_heapptr(ctx, -1);
        const std::string proto_name = core::Str()
                                       << DUK_HIDDEN_SYMBOL("proto") << name;
        duk_put_global_string(ctx, proto_name.c_str());  // empty stack

        classIds.insert(std::make_pair(
                bind.id, std::make_shared<RegisteredClass>(name, prototype)));
    }

    Function*
    Duk::CreateFunction(const std::shared_ptr<Function>& func)
    {
        functions.emplace_back(func);
        return func.get();
    }

    Duk::~Duk()
    {
        duk_destroy_heap(ctx);
    }

    RegisteredClass*
    Duk::TypeToProto(core::TypeId id CLASS_ARG(core::TypeName name))
    {
        const auto found = classIds.find(id);
        if(found == classIds.end())
        {
            const std::string error
                    = core::Str() << "class not added" CLASS_NAME(": " << name);
            DIE(error.c_str());
            return nullptr;
        }
        else
        {
            return found->second.get();
        }
    }
}  // namespace euphoria::duk