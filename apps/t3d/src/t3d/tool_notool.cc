#include "t3d/tool_notool.h"


#include "imgui/imgui.h"

#include "t3d/editor.h"


namespace euphoria::t3d
{
    bool
    NoTool::IsBusy(Editor*)
    {
        return false;
    }


    void
    NoTool::Step(Editor*)
    {}


    void
    NoTool::OnMouse(Editor* editor, core::MouseButton button, bool down)
    {
        if(down) { return; }
        if(button != core::MouseButton::left) { return; }
        auto ray = editor->camera
                        .clip_to_world_ray(
                                editor->viewport.to_clip_coord(editor->mouse))
                        .get_normalized();
        
        editor->SetAllSelected(false);
        auto hits = editor->Raycast(ray);

        for(auto h: hits)
        {
            h->is_selected = true;
        }
    }


    void
    NoTool::OnKey(Editor*, core::key, bool)
    {}


    void
    NoTool::OnScroll(Editor*, const core::vec2i&)
    {}


    void
    NoTool::OnEditor(Editor*)
    {
        ImGui::Text("<No tool>");
    }
}
