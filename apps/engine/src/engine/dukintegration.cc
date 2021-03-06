#include "engine/dukintegration.h"

#include <map>
#include <functional>

#include "core/ecs-systems.h"
#include "core/random.h"
#include "core/log.h"
#include "core/sol.h"
#include "core/cint.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"


namespace euphoria::engine
{

    template<typename T>
    std::vector<T> get_vector(const sol::table& table)
    {
        std::vector<T> r;
        r.reserve(table.size());
        for(unsigned int i=0; i<table.size(); i+=1)
        {
            r.emplace_back(table[i+1]);
        }
        return r;
    }


    void report_first_error(const sol::protected_function_result& val, lua* ctx, const std::string& action, const std::string& where)
    {
        if (val.valid())
        {
            return;
        }

        const sol::error err = val;
        const auto message = fmt::format("Failed to {0} in {1}: {2}", action, where, err.what());
        LOG_ERROR("{0}", message);
        if (!ctx->has_error)
        {
            ctx->has_error = true;
            ctx->error = message;
        }
    }


    struct script_update_system
        : core::ecs::component_system
        , core::ecs::component_system_updater
    {
        using update_function = sol::protected_function; // void(double)

        lua* duk;
        update_function func;

        script_update_system(const std::string& name, lua* d, update_function f)
            : component_system(name)
            , duk(d)
            , func(f)
        {
            ASSERT(func);
        }

        void
        update(core::ecs::registry*, float dt) const override
        {
            auto result = func(core::c_float_to_double(dt));
            report_first_error(result, duk, "update", name);
        }

        void
        register_callbacks(core::ecs::systems* systems) override
        {
            systems->updater.add(this);
        }
    };


    struct script_init_system
        : public core::ecs::component_system
        , public core::ecs::component_system_initializer
    {
    	using init_function = sol::protected_function; // void(entity_id)

        core::ecs::registry* reg;
        lua* duk;
        std::vector<core::ecs::component_id> types;
        init_function func;


        script_init_system
        (
                const std::string& name,
                lua* d,
                core::ecs::registry* r,
                const std::vector<core::ecs::component_id>& t,
                init_function f
        )
            : component_system(name)
            , reg(r)
            , duk(d)
            , types(t)
            , func(f)
        {
        }

        void
        on_add(core::ecs::entity_id entity) const override
        {
            ASSERT(reg);
            ASSERT(!types.empty());

            for(const auto& type: types)
            {
                const auto component = reg->get_component(entity, type);
                const bool has_component = component != nullptr;

                if(!has_component)
                {
                    return;
                }
            }
            auto result = func(entity);
            report_first_error(result, duk, "init", name);
        }

        void
        register_callbacks(core::ecs::systems* systems) override
        {
            systems->initializer.add(this);
        }
    };


    struct script_systems
    {
        core::ecs::systems* systems;
        lua* duk;

        explicit script_systems(core::ecs::systems* s, lua* d)
            : systems(s)
            , duk(d)
        {
        }

        void
        add_update(const std::string& name, script_update_system::update_function func)
        {
            systems->add_and_register(std::make_shared<script_update_system>(name, duk, func));
        }

        void
        add_init
        (
            const std::string& name,
            core::ecs::registry* reg,
            const std::vector<core::ecs::component_id>& types,
            script_init_system::init_function func
        )
        {
            systems->add_and_register(std::make_shared<script_init_system>(name, duk, reg, types, func));
        }
    };

    struct script_integration_pimpl
    {
        script_integration_pimpl
        (
                core::ecs::systems* sys,
                core::ecs::world* world,
                lua* duk,
                object_creator* creator,
                engine::components* cc,
                camera_data* cam
        )
            : systems(sys, duk)
            , registry(&world->reg, cc)
            , input(duk->state["input"].get_or_create<sol::table>())
            , world(world)
            , creator(creator)
            , components(cc)
            , camera(cam)
        {
        }

        void
        integrate(lua* duk)
        {
            auto systems_table = duk->state["systems"].get_or_create<sol::table>();
            systems_table["add_update"] = [&](const std::string& name, sol::function func)
            {
                systems.add_update(name, func);
            };
            systems_table["on_init"] = [&]
            (
                const std::string& name,
                sol::table types,
                sol::function func
            )
            {
                auto vtypes = get_vector<core::ecs::component_id>(types);
                systems.add_init(name, &world->reg, vtypes, func);
            };

            auto math_table = duk->state["math"].get_or_create<sol::table>();
            math_table["new_random"] = [&]() { return std::make_shared<core::random>(); };

            auto templates_table = duk->state["templates"].get_or_create<sol::table>();
            templates_table["find"] = [&](const std::string& name)
            {
                return creator->find_template(name);
            };

            auto template_type = duk->state.new_usertype<object_template>("template");
            template_type["create"] = [&, duk](object_template* t)
            {
                return t->create_object(object_creation_arguments{world, &registry, duk, duk});
            };

            auto camera_table = duk->state["camera"].get_or_create<sol::table>();
            camera_table["get_rect"] = [&]() { return &camera->screen; };

            duk->state.new_usertype<custom_arguments>
            (
                "custom_arguments",
                "get_number", [](const custom_arguments& args, const std::string& name) -> double
                {
                    const auto f = args.numbers.find(name);
                    if(f == args.numbers.end())
                    {
                        return 0.0;
                    }
                    else
                    {
                        return core::c_float_to_double(f->second);
                    }
                }
            );

            auto registry_table = duk->state["registry"].get_or_create<sol::table>();
            registry_table["entities"] = [&](sol::table types)
            {
                auto vec = get_vector<core::ecs::component_id>(types);
                if(vec.empty())
                {
                    throw std::runtime_error("no entities specified");
                }
                return registry.entity_view(vec);
            };
            registry_table["get_sprite_id"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["destroy_entity"] = [&](core::ecs::entity_id entity)
            {
                registry.destroy_entity(entity);
            };
            registry_table["get_position2_id"] = [&]()
            {
                LOG_INFO("Getting position 2d");
                return registry.components->position2;
            };
            registry_table["get_sprite_id"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["create"] = sol::overload
            (
                [&](const std::string& name, sol::function setup)
                {
                    return registry.create_new_id(name, setup);
                },
                [&](const std::string& name)
                {
                    return registry.create_new_id(name);
                }
            );
            registry_table["get"] = [&](core::ecs::entity_id ent, core::ecs::component_id comp)
            {
                return registry.get_property(ent, comp);
            };
            registry_table["set"] = [&](core::ecs::entity_id ent, core::ecs::component_id comp, sol::table val)
            {
                registry.set_property(ent, comp, val);
            };
            registry_table["get_sprite"] = [&](core::ecs::component_id ent)
            {
                return registry.get_component_or_null<component_sprite>(ent, components->sprite);
            };
            registry_table["get_position2"] = [&](core::ecs::component_id ent)
            {
                return registry.get_component_or_null<component_position2>(ent, components->position2);
            };
            registry_table["get_position2_vec"] = [&](core::ecs::component_id ent)
            {
                auto* c = registry.get_component_or_null<component_position2>(ent, components->position2);
                return c == nullptr ? nullptr : &c->pos;
            };

            duk->state.new_usertype<component_position2>("component_position2", "vec", &component_position2::pos);

            duk->state.new_usertype<component_sprite>
            (
                "component_sprite",
                "get_rect", [](const component_sprite& sp, const component_position2& p)
                {
                    return get_sprite_rect(p.pos, *sp.texture);
                }
            );

            auto rect_type = duk->state.new_usertype<core::rectf>("rectf", sol::no_constructor);
            rect_type["contains"] = sol::overload
            (
                [](const core::rectf& r, const core::rectf& rr) -> bool
                {
                    return r.contains_exclusive(rr);
                },
                [](const core::rectf& r, const core::vec2f& p) -> bool
                {
                    return r.contains_exclusive(p);
                },
                [](const core::rectf& r, double x, double y) -> bool
                {
                    return r.contains_exclusive(core::c_double_to_float(x), core::c_double_to_float(y));
                }
            );
            rect_type["get_height"] = [](const core::rectf& r) -> double { return core::c_float_to_double(r.get_height()); };
            rect_type["get_width"] = [](const core::rectf& r) -> double { return core::c_float_to_double(r.get_width()); };

            auto random_type = duk->state.new_usertype<core::random>("random");
            random_type["next_float01"] = [](core::random& r) -> double { return core::c_float_to_double(r.get_next_float01()); };
            random_type["next_range_float"] = [](core::random& r, double f) -> double
            {
                return core::c_float_to_double(get_random_in_range(&r, core::c_double_to_float(f)));
            };
            random_type["next_bool"] = &core::random::get_next_bool;
            random_type["next_point2"] = [](core::random& r, core::rectf& rect) -> core::vec2f
            {
                return rect.get_random_point(&r);
            };
        }

        script_systems systems;
        script_registry registry;
        sol::table input;
        core::ecs::world* world;
        object_creator* creator;
        engine::components* components;
        camera_data* camera;
    };


    script_integration::script_integration
    (
            core::ecs::systems* systems,
            core::ecs::world* reg,
            lua* duk,
            object_creator* creator,
            components* components,
            camera_data* camera
    )
    {
        pimpl = std::make_unique<script_integration_pimpl>
        (
            systems,
            reg,
            duk,
            creator,
            components,
            camera
        );
        pimpl->integrate(duk);
    }


    script_integration::~script_integration()
    {
        clear();
    }


    void
    script_integration::clear()
    {
        pimpl.reset();
    }


    script_registry&
    script_integration::get_registry()
    {
        ASSERT(pimpl);
        return pimpl->registry;
    }


    void
    script_integration::bind_keys(const input_system &input)
    {
        input.set(&pimpl->input);
    }
}

TYPEID_SETUP_TYPE(euphoria::core::random);
TYPEID_SETUP_TYPE(euphoria::engine::object_template);
TYPEID_SETUP_TYPE(euphoria::core::rect<float>);
