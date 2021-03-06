#include "engine/dukregistry.h"

#include <algorithm>

#include "core/assert.h"
#include "core/log.h"


namespace euphoria::engine
{
    struct script_component : public core::ecs::component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(script_component)

        sol::table val;
    };

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(script_component)

    script_registry::script_registry(core::ecs::registry* r, engine::components* c)
        : reg(r)
        , components(c)
    {
    }

    core::ecs::component_id
    script_registry::create_new_id
    (
        const std::string& name,
        const script_registry::creation_callback& fv
    )
    {
        const auto id = reg->register_new_component_type(name);
        script_components[id] = fv;
        return id;
    }

    core::ecs::component_id
    script_registry::create_new_id
    (
        const std::string& name
    )
    const
    {
        const auto id = reg->register_new_component_type(name);
        return id;
    }

    core::result<core::ecs::component_id>
    script_registry::get_custom_component_by_name
    (
        const std::string& name
    )
    const
    {
        return reg->get_custom_component_by_name(name);
    }

    std::vector<core::ecs::entity_id>
    script_registry::entity_view(const std::vector<core::ecs::component_id>& types) const
    {
        return reg->get_entities_with_components(types);
    }

    sol::table
    script_registry::get_property
    (
        core::ecs::entity_id ent,
        core::ecs::component_id comp
    )
    {
        ASSERT(script_components.find(comp) != script_components.end());
        auto c = reg->get_component(ent, comp);
        if(c == nullptr)
        {
            return sol::table{};
        }
        else
        {
            return static_cast<script_component*>(c.get())->val;
        }
    }

    script_component*
    get_script_component(core::ecs::registry *reg, core::ecs::entity_id ent, core::ecs::component_id comp)
    {
        auto c = reg->get_component(ent, comp);
        if(c == nullptr)
        {
            auto d = std::make_shared<script_component>();
            reg->add_component_to_entity(ent, comp, d);
            return d.get();
        }
        else
        {
            return static_cast<script_component*>(c.get());
        }
    }

    void
    script_registry::set_property
    (
        core::ecs::entity_id ent,
        core::ecs::component_id comp,
        sol::table value
    )
    const
    {
        script_component* component = get_script_component(reg, ent, comp);
        component->val = value;
    }

    sol::table
    script_registry::create_component
    (
            core::ecs::component_id comp,
            lua* ctx,
            const custom_arguments& arguments
    )
    {
        const auto name = reg->get_component_name(comp);

        auto res = script_components.find(comp);
        if(res == script_components.end())
        {
            // no custom function, use use a empty table...?
            // or perhaps use null?
            return sol::table{ctx->state, sol::create};
        }

        auto val = res->second(arguments);
        if(val.valid())
        {
            sol::table ret = val;
            return ret;
        }
        else
        {
            sol::error err = val;
            const auto message = fmt::format("Failed to call create for component {0}({1}): {2}", name, comp, err.what());
            LOG_ERROR("{0}", message);
            if(!ctx->has_error)
            {
                ctx->has_error = true;
                ctx->error = message;
            }
            return sol::table{ctx->state, sol::create};
        }
    }

    void
    script_registry::destroy_entity(core::ecs::entity_id id) const
    {
        reg->destroy_entity(id);
    }
}

TYPEID_SETUP_TYPE(euphoria::engine::script_component);
