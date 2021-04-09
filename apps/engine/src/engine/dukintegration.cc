#include "engine/dukintegration.h"

#include <map>
#include <functional>

#include "core/ecs-systems.h"
#include "core/random.h"
#include "core/log.h"
#include "core/sol.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.integration")

namespace euphoria::engine
{

    template<typename T>
    std::vector<T> GetVector(const sol::table& table)
    {
        std::vector<T> r;
        r.reserve(table.size());
        for(unsigned int i=0; i<table.size(); i+=1)
        {
            r.emplace_back(table[i+1]);
        }
        return r;
    }


    void ReportFirstError(const sol::protected_function_result& val, Sol* ctx,const std::string& action, const std::string& where)
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


    struct DukUpdateSystem
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemUpdate
    {
        // using UpdateFunction = std::function<void(float)>;
        using UpdateFunction = sol::protected_function;

        Sol* duk;
        UpdateFunction func;

        DukUpdateSystem(const std::string& name, Sol* d, UpdateFunction f)
            : ComponentSystem(name)
            , duk(d)
            , func(f)
        {
            ASSERT(func);
        }

        void
        Update(core::ecs::Registry*, float dt) const override
        {
            auto result = func(dt);
            ReportFirstError(result, duk, "update", name);
        }

        void
        RegisterCallbacks(core::ecs::Systems* systems) override
        {
            systems->update.Add(this);
        }
    };


    struct DukInitSystem
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemInit
    {
    	// using InitFunction = std::function<void(core::ecs::EntityId)>;
    	using InitFunction = sol::protected_function;
    	
        core::ecs::Registry* reg;
        Sol* duk;
        std::vector<core::ecs::ComponentId> types;
        InitFunction func;


        DukInitSystem
        (
            const std::string& name,
            Sol* d,
            core::ecs::Registry* r,
            const std::vector<core::ecs::ComponentId>& t,
            InitFunction f
        )
            : ComponentSystem(name)
            , reg(r)
            , duk(d)
            , types(t)
            , func(f)
        {
        }

        void
        OnAdd(core::ecs::EntityId entity) const override
        {
            ASSERT(reg);
            ASSERT(!types.empty());

            for(const auto& type: types)
            {
                const auto component = reg->GetComponent(entity, type);
                const bool has_component = component != nullptr;

                if(!has_component)
                {
                    return;
                }
            }
            auto result = func(entity);
            ReportFirstError(result, duk, "init", name);
        }

        void
        RegisterCallbacks(core::ecs::Systems* systems) override
        {
            systems->init.Add(this);
        }
    };


    struct DukSystems
    {
        core::ecs::Systems* systems;
        Sol* duk;

        explicit DukSystems(core::ecs::Systems* s, Sol* d)
            : systems(s)
            , duk(d)
        {
        }

        void
        AddUpdate(const std::string& name, DukUpdateSystem::UpdateFunction func)
        {
            systems->AddAndRegister(std::make_shared<DukUpdateSystem>(name, duk, func));
        }

        void
        AddInit
        (
            const std::string& name,
            core::ecs::Registry* reg,
            const std::vector<core::ecs::ComponentId>& types,
            DukInitSystem::InitFunction func
        )
        {
            systems->AddAndRegister(std::make_shared<DukInitSystem>(name, duk, reg, types, func));
        }
    };

    struct DukIntegrationPimpl
    {
        DukIntegrationPimpl
        (
            core::ecs::Systems* sys,
            core::ecs::World* world,
            Sol* duk,
            ObjectCreator* creator,
            Components* components,
            CameraData*    cam
        )
            : systems(sys, duk)
            , registry(&world->reg, components)
            , input(duk->lua["Input"].get_or_create<sol::table>())
            , world(world)
            , creator(creator)
            , components(components)
            , camera(cam)
        {
        }

        void
        Integrate(Sol* duk)
        {
            auto systems_table         = duk->lua["Systems"].get_or_create<sol::table>();
            systems_table["AddUpdate"] = [&](const std::string& name, sol::function func)
            {
                systems.AddUpdate(name, func);
            };
            systems_table["OnInit"] = [&]
            (
                const std::string&              name,
                sol::table types,
                sol::function                   func
            )
            {
              auto vtypes = GetVector<core::ecs::ComponentId>(types);
              systems.AddInit(name, &world->reg, vtypes, func);
            };

            auto math_table         = duk->lua["Math"].get_or_create<sol::table>();
            math_table["NewRandom"] = [&]() { return std::make_shared<core::Random>(); };

            auto templates_table    = duk->lua["Templates"].get_or_create<sol::table>();
            templates_table["Find"] = [&](const std::string& name)
            {
                return creator->FindTemplate(name);
            };

            duk->lua.new_usertype<ObjectTemplate>("Template");
            duk->lua["Template"]["Create"] = [&, duk](ObjectTemplate* t)
            {
                return t->CreateObject(ObjectCreationArgs{world, &registry, duk, duk});
            };

            auto camera_table       = duk->lua["Camera"].get_or_create<sol::table>();
            camera_table["GetRect"] = [&]() { return &camera->screen; };

            duk->lua.new_usertype<CustomArguments>
            (
                "CustomArguments",
                "GetNumber", [](const CustomArguments& args, const std::string& name) -> float
                {
                      const auto f = args.numbers.find(name);
                      if(f == args.numbers.end())
                      {
                            return 0;
                      }
                      else
                      {
                            return f->second;
                      }
                }
            );

            auto registry_table        = duk->lua["Registry"].get_or_create<sol::table>();
            registry_table["Entities"] = [&](sol::table types)
            {
                return registry.EntityView(GetVector<core::ecs::ComponentId>(types));
            };
            registry_table["GetSpriteId"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["DestroyEntity"] = [&](core::ecs::EntityId entity)
            {
                registry.DestroyEntity(entity);
            };
            registry_table["GetPosition2Id"] = [&]()
            {
                LOG_INFO("Getting position 2d");
                return registry.components->position2;
            };
            registry_table["GetSpriteId"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["New"] = sol::overload
            (
                [&](const std::string& name, sol::function setup)
                {
                    return registry.CreateNewId(name, setup);
                },
                [&](const std::string& name)
                {
                    return registry.CreateNewId(name);
                }
            );
            registry_table["Get"] = [&](core::ecs::EntityId ent, core::ecs::ComponentId comp)
            {
                return registry.GetProperty(ent, comp);
            };
            registry_table["Set"] = [&](core::ecs::EntityId ent, core::ecs::ComponentId comp, sol::table val)
            {
                registry.SetProperty(ent, comp, val);
            };
            registry_table["GetSprite"] = [&](core::ecs::ComponentId ent)
            {
                return registry.GetComponentOrNull<CSprite>(ent, components->sprite);
            };
            registry_table["GetPosition2"] = [&](core::ecs::ComponentId ent)
            {
                return registry.GetComponentOrNull<CPosition2>(ent, components->position2);
            };
            registry_table["GetPosition2vec"] = [&](core::ecs::ComponentId ent)
            {
                auto c = registry.GetComponentOrNull<CPosition2>(ent, components->position2);
                return c == nullptr ? nullptr : &c->pos;
            };

            duk->lua.new_usertype<CPosition2>("CPosition2", "vec", &CPosition2::pos);

            duk->lua.new_usertype<CSprite>
            (
                "CSprite",
                "GetRect", [](const CSprite& sp, const CPosition2& p)
                {
                    return GetSpriteRect(p.pos, *sp.texture);
                }
            );

            auto rect_type = duk->lua.new_usertype<core::Rectf>("Rectf", sol::no_constructor);
            rect_type["Contains"] = sol::overload
            (
                [](const core::Rectf& r, const core::Rectf& rr) -> bool
                {
                    return r.ContainsExclusive(rr);
                },
                [](const core::Rectf& r, const core::vec2f& p) -> bool
                {
                    return r.ContainsExclusive(p);
                },
                [](const core::Rectf& r, float x, float y) -> bool
                {
                    return r.ContainsExclusive(x, y);
                }
            );
            rect_type["GetHeight"] = &core::Rectf::GetHeight;
            rect_type["GetWidth"] = &core::Rectf::GetWidth;

            auto random_type = duk->lua.new_usertype<core::Random>("Random");
            random_type["NextFloat01"] = &core::Random::NextFloat01;
            random_type["NextRangeFloat"] = [](core::Random& r, float f) -> float
            {
                return r.NextRange(f);
            };
            random_type["NextBool"] = &core::Random::NextBool;
            random_type["NextPoint2"] = [](core::Random& r, core::Rectf& rect) -> core::vec2f
            {
                return rect.RandomPoint(&r);
            };
        }

        DukSystems systems;
        DukRegistry registry;
        sol::table     input;
        core::ecs::World* world;
        ObjectCreator* creator;
        Components* components;
        CameraData* camera;
    };


    DukIntegration::DukIntegration
    (
        core::ecs::Systems* systems,
        core::ecs::World* reg,
        Sol* duk,
        ObjectCreator* creator,
        Components* components,
        CameraData* camera
    )
    {
        pimpl.reset
        (
            new DukIntegrationPimpl
            (
                systems,
                reg,
                duk,
                creator,
                components,
                camera
            )
        );
        pimpl->Integrate(duk);
    }


    DukIntegration::~DukIntegration()
    {
        Clear();
    }


    void
    DukIntegration::Clear()
    {
        pimpl.reset();
    }


    DukRegistry&
    DukIntegration::Registry()
    {
        ASSERT(pimpl);
        return pimpl->registry;
    }


    void
    DukIntegration::BindKeys(Sol* duk, const Input& input)
    {
        input.Set(&pimpl->input);
    }
}

TYPEID_SETUP_TYPE(euphoria::core::Random);
TYPEID_SETUP_TYPE(euphoria::engine::ObjectTemplate);
TYPEID_SETUP_TYPE(euphoria::core::Rect<float>);
