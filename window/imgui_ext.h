#ifndef EUPHORIA_IMGUI_H
#define EUPHORIA_IMGUI_H

#include <string>

#include "core/vec2.h"

namespace euphoria::core
{
    struct Angle;
    struct Rgb;
}  // namespace euphoria::core

namespace euphoria::render
{
    struct Texture2d;
}

struct ImVec2;


namespace euphoria::window
{
    bool
    InputText(const char* label, std::string* str);

    void
    ImguiLabel(const std::string& str);

    void
    ImguiAngleSlider(
            const char* const name,
            core::Angle*      angle,
            float             mindeg,
            float             maxdeg);

    void
    ImguiImage(render::Texture2d* texture);

    void
    ImGuiColorEdit3(const char* const name, core::Rgb* rgb);

    enum class ImguiCorner
    {
        TopLeft     = 0,
        TopRight    = 1,
        BottomLeft  = 2,
        BottomRight = 3,
        Center      = 4
    };

    bool
    BeginFixedOverlay(ImguiCorner corner, const std::string& title);

    ImVec2
    C(const core::vec2f& v);

    core::vec2f
    C(const ImVec2& v);

    struct ImguiDisabled
    {
        ImguiDisabled();
        ~ImguiDisabled();
    };

    bool
    ImguiSelectableOrDisabled(bool enabled, const char* label);
}  // namespace euphoria::window

#endif  // EUPHORIA_IMGUI_H
