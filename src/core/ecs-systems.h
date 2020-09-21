#ifndef EUPHORIA_COMPONENTSYSTEM_H
#define EUPHORIA_COMPONENTSYSTEM_H

#include <memory>

#include <string>
#include <vector>

#include "core/ecs.h"
#include "core/ecs-id.h"

namespace euphoria::render
{
    // its horrible to reference in another module,
    // but for now we only need a pointer to pass around
    // todo: fix this by merging in more of the renderer into core?
    struct SpriteRenderer;
}  // namespace euphoria::render

namespace euphoria::core::ecs
{
    typedef Registry EntReg;

    struct Systems;

    // only a base class container
    struct ComponentSystem
    {
        explicit ComponentSystem(const std::string& the_name);
        virtual ~ComponentSystem() = default;

        virtual void
        RegisterCallbacks(Systems* systems)
                = 0;

        // for debug purposes
        const std::string name;
    };

    struct ComponentSystemStore
    {
        ComponentSystemStore() = default;

        void
        Add(std::shared_ptr<ComponentSystem> system);

    private:
        std::vector<std::shared_ptr<ComponentSystem>> systems;
    };

    struct ComponentSystemUpdate
    {
        virtual ~ComponentSystemUpdate() = default;

        virtual void
        Update(EntReg* reg, float dt) const = 0;
    };

    struct ComponentSystemInit
    {
        virtual ~ComponentSystemInit() = default;

        virtual void
        OnAdd(EntityId entity) const = 0;
    };

    struct ComponentSystemSpriteDraw
    {
        virtual ~ComponentSystemSpriteDraw() = default;

        virtual void
        Draw(EntReg* reg, render::SpriteRenderer* renderer) const = 0;
    };

    template <typename TSystem>
    struct SystemStore
    {
        void
        Add(TSystem* system)
        {
            systems.emplace_back(system);
        }

    protected:
        std::vector<TSystem*> systems;
    };

    struct ComponentSystemUpdateStore
        : public SystemStore<ComponentSystemUpdate>
    {
        void
        Update(EntReg* reg, float dt) const;
    };

    struct ComponentSystemInitStore : public SystemStore<ComponentSystemInit>
    {
        void
        OnAdd(EntityId ent) const;
    };

    struct ComponentSystemSpriteDrawStore
        : public SystemStore<ComponentSystemSpriteDraw>
    {
        void
        Draw(EntReg* reg, render::SpriteRenderer* renderer) const;
    };

    struct Systems
    {
        void
        AddAndRegister(std::shared_ptr<ComponentSystem> system);

        // stores the system
        ComponentSystemStore store;

        // system references for various global callbacks
        ComponentSystemUpdateStore     update;
        ComponentSystemInitStore       init;
        ComponentSystemSpriteDrawStore spriteDraw;
    };

    struct World
    {
        EntReg   reg;
        Systems* systems;

        explicit World(Systems* sys);

        void
        Update(float dt);

        void
        Draw(render::SpriteRenderer* renderer);
    };

}  // namespace euphoria::core::ecs

#endif  // EUPHORIA_COMPONENTSYSTEM_H