#include "t3d/editor.h"


#include "core/intersection.h"

#include "render/actor.h"

#include "t3d/tool.h"
#include "t3d/tilelibrary.h"


namespace euphoria::t3d
{
    Editor::Editor
    (
        Grid* agrid,
        render::World* aworld,
        TileLibrary* atile_library
    )
        : grid(agrid)
        , world(aworld)
        , tile_library(atile_library)
        , camera(core::mat4f::identity(), core::mat4f::identity())
        , viewport(core::recti::from_width_height(10, 10))
        , mouse(core::vec2i {0, 0})
    {}


    std::shared_ptr<PlacedMesh>
    Editor::GetFirstSelectedOrNull()
    {
        for(auto act : actors)
        {
            if(act->is_selected)
            {
                return act;
            }
        }

        return nullptr;
    }


    void
    Editor::SetAllSelected(bool is_selected)
    {
        for(auto actor: actors)
        {
            actor->is_selected = is_selected;
        }
    }


    std::vector<std::shared_ptr<PlacedMesh>>
    Editor::Raycast(const core::unit_ray3f& ray)
    {
        std::vector<std::shared_ptr<PlacedMesh>> r;

        for(auto a: actors)
        {
            const auto collision = core::GetIntersection(ray, a->tile->aabb.offset_copy(a->actor->GetPosition()));
            
            if(collision.intersected)
            {
                r.emplace_back(a);
            }
        }

        return r;
    }


    // current tool callbacks
    bool
    Editor::IsBusy()
    {
        return tools.GetCurrentTool()->IsBusy(this);
    }


    void
    Editor::Step()
    {
        tools.GetCurrentTool()->Step(this);
    }


    void
    Editor::OnMouse(core::MouseButton button, bool down)
    {
        tools.GetCurrentTool()->OnMouse(this, button, down);
    }


    void
    Editor::OnKey(core::Key key, bool down)
    {
        tools.GetCurrentTool()->OnKey(this, key, down);
    }


    void
    Editor::OnScroll(const core::vec2i& scroll)
    {
        tools.GetCurrentTool()->OnScroll(this, scroll);
    }


    void
    Editor::OnEditor()
    {
        tools.GetCurrentTool()->OnEditor(this);
    }
}
