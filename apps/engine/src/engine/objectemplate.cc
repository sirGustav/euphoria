#include "engine/objectemplate.h"

#include "core/stdutils.h"
#include "core/stringmerger.h"
#include "core/vfs_path.h"
#include "core/log.h"
#include "core/proto.h"

#include "engine/components.h"
#include "engine/dukregistry.h"

#include "gaf_game.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.templates")

namespace euphoria::engine
{
    ////////////////////////////////////////////////////////////////////////////////

    ObjectCreationArgs::ObjectCreationArgs
    (
        core::ecs::world* aworld,
        DukRegistry*      areg,
        Sol*     actx,
        Sol*         aduk
    )
        : world(aworld)
        , reg(areg)
        , ctx(actx)
        , duk(aduk)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    struct PositionComponentCreator : public ComponentCreator
    {
    public:
        core::vec2f p;
        Components* components;

        PositionComponentCreator(const core::vec2f& pp, Components* components)
            : p(pp), components(components)
        {}

        [[nodiscard]]
        static
        std::shared_ptr<PositionComponentCreator>
        Create(const game::vec2f& p, Components* components)
        {
            return std::make_shared<PositionComponentCreator>(
                    core::vec2f {p.x, p.y}, components);
        }

        void
        CreateComponent(const ObjectCreationArgs& args, core::ecs::entity_id ent)
                override
        {
            auto c = std::make_shared<CPosition2>();
            c->pos = p;
            args.world->reg.add_component_to_entity(ent, components->position2, c);
        }
    };

    struct SpriteComponentCreator : public ComponentCreator
    {
    public:
        std::shared_ptr<render::texture2d> texture;
        Components* components;

        explicit SpriteComponentCreator(Components* c) : components(c) {}

        [[nodiscard]]
        static
        std::shared_ptr<SpriteComponentCreator>
        Create
        (
            const game::Sprite& sprite,
            render::TextureCache* cache,
            Components* components
        )
        {
            auto ptr     = std::make_shared<SpriteComponentCreator>(components);
            ptr->texture = cache->GetTexture
            (
                core::vfs::FilePath::FromScript(sprite.path)
            );
            return ptr;
        }

        void
        CreateComponent(const ObjectCreationArgs& args, core::ecs::entity_id ent)
                override
        {
            auto c     = std::make_shared<CSprite>();
            c->texture = texture;
            args.world->reg.add_component_to_entity(ent, components->sprite, c);
        }
    };


    struct CustomComponentCreator : public ComponentCreator
    {
    public:
        core::ecs::component_id comp;
        CustomArguments arguments;

        [[nodiscard]]
        static
        std::shared_ptr<CustomComponentCreator>
        Create
        (
            const std::string&            name,
            core::ecs::component_id        id,
            const std::vector<game::Var>& arguments
        )
        {
            auto ptr  = std::make_shared<CustomComponentCreator>();
            ptr->comp = id;
            for(const auto& a: arguments)
            {
                if(a.number != nullptr)
                {
                    ptr->arguments.numbers[a.name] = a.number->value;
                }
                else
                {
                    LOG_ERROR
                    (
                        "Invalid type for custom argument {0} for type {1}",
                        a.name,
                        name
                    );
                }
            }
            return ptr;
        }

        void
        CreateComponent(const ObjectCreationArgs& args, core::ecs::entity_id ent)
                override
        {
            auto val = args.reg->CreateComponent(comp, args.ctx, arguments);
            args.reg->SetProperty(ent, comp, val);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////


    std::shared_ptr<ComponentCreator>
    CreateCreator
    (
        const game::Component& comp,
        DukRegistry*           reg,
        render::TextureCache*  cache,
        Components*            components
    )
    {
        if(comp.position)
        {
            return PositionComponentCreator::Create(*comp.position, components);
        }
        else if(comp.sprite)
        {
            return SpriteComponentCreator::Create
            (
                *comp.sprite,
                cache,
                components
            );
        }
        else if(comp.custom)
        {
            const auto& s = *comp.custom;
            core::ecs::component_id id;
            if(reg->GetCustomComponentByName(s.name, &id))
            {
                return CustomComponentCreator::Create(s.name, id, s.arguments);
            }
            else
            {
                LOG_ERROR
                (
                    "No custom component named {0} was added.",
                    s.name
                );
                return nullptr;
            }
        }

        LOG_ERROR("No component found, bug!!!");
        return nullptr;
    }


    void
    LoadObjectTemplate
    (
        ObjectTemplate* ot,
        const game::Template& ct,
        DukRegistry* reg,
        render::TextureCache* cache,
        Components* components
    )
    {
        for(const auto& comp: ct.components)
        {
            auto c = CreateCreator(comp, reg, cache, components);
            if(c != nullptr)
            {
                ot->components.emplace_back(c);
            }
        }
    }


    core::ecs::entity_id
    ObjectTemplate::CreateObject(const ObjectCreationArgs& args)
    {
        auto ent = args.world->reg.create_new_entity();
        for(const auto& c: components)
        {
            c->CreateComponent(args, ent);
        }

        // todo(Gustav): run init function here
        args.world->reg.post_create(ent);

        return ent;
    }


    ////////////////////////////////////////////////////////////////////////////////


    void
    LoadTemplatesButOnlyNames(const game::Game& json, ObjectCreator* temp)
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<ObjectTemplate>();
            temp->templates.insert(std::make_pair(t.name, o));
        }
    }


    void
    LoadTemplates(
            const game::Game&     json,
            ObjectCreator*        temp,
            DukRegistry*          reg,
            render::TextureCache* cache,
            Components*           components)
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<ObjectTemplate>();

            auto fr = temp->templates.find(t.name);
            if(fr == temp->templates.end())
            {
                temp->templates.insert(std::make_pair(t.name, o));
            }
            else
            {
                o = fr->second;
            }

            LoadObjectTemplate(o.get(), t, reg, cache, components);
        }
    }


    ObjectTemplate*
    ObjectCreator::FindTemplate(const std::string& name)
    {
        auto result = templates.find(name);
        if(result == templates.end())
        {
            LOG_ERROR
            (
                "Failed to find template named {0}, could be {1}.",
                name,
                core::string_mergers::english_or.
                    merge(core::get_keys(templates))
            );
            return nullptr;
        }

        return result->second.get();
    }

    ////////////////////////////////////////////////////////////////////////////////
}  // namespace euphoria::engine
