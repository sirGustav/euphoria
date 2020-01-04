#ifndef GUI_SKIN_H
#define GUI_SKIN_H

#include <string>
#include <memory>
#include <optional>

#include "core/rgb.h"
#include "core/interpolate.h"
#include "core/vfs_path.h"


namespace euphoria::render
{
    struct Font;
}


namespace euphoria::gui
{
    struct Interpolation
    {
        core::InterpolationType type = core::InterpolationType::Linear;
        float time = 0.0f;
    };


    struct ButtonState
    {
    public:
        // std::string image;
        float scale = 0;
        core::Rgb image_color {0.0f};
        core::Rgb text_color {1.0f};
        float dx = 0;
        float dy = 0;

        Interpolation interpolation_position;
        Interpolation interpolation_color;
        Interpolation interpolation_size;
    };


    struct Skin
    {
    public:
        ~Skin();

        std::string name;

        std::shared_ptr<render::Font> font;

        float text_size = 30.0f;

        std::optional<core::vfs::FilePath> button_image;

        ButtonState button_idle;
        ButtonState button_hot;
        ButtonState button_active_hot;
    };
}  // namespace euphoria::gui

#endif  // GUI_SKIN_H
