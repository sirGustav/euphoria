#pragma once

#include <memory>

#include "core/sol.h"
#include "core/noncopyable.h"

namespace euphoria::core::ecs
{
    struct systems;
    struct world;
}

namespace euphoria::engine
{
    struct input_system;
    struct script_registry;
    struct object_creator;
    struct script_integration_pimpl;
    struct camera_data;
    struct components;


    struct script_integration
    {
    public:
        script_integration
        (
                core::ecs::systems* systems,
                core::ecs::world* reg,
                lua* duk,
                object_creator* creator,
                components* components,
                camera_data* camera
        );
        ~script_integration();

        NONCOPYABLE(script_integration);

        void
        clear();

        script_registry&
        get_registry();

        void
        bind_keys(const input_system& input);

    private:
        std::unique_ptr<script_integration_pimpl> pimpl;
    };
}
