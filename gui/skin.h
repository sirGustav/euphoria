#ifndef GUI_SKIN_H
#define GUI_SKIN_H

#include <string>
#include <memory>

#include "core/rgb.h"
#include "core/interpolate.h"

namespace euphoria::render
{
    struct Font;
}


namespace euphoria::gui
{
    struct ButtonState
    {
    public:
        ButtonState();
        ~ButtonState();

        // std::string image;
        float     scale;
        core::Rgb image_color;
        core::Rgb text_color;
        float     dx;
        float     dy;

        core::InterpolationType interpolationPosition;
        float                   interpolationPositionTime;

        core::InterpolationType interpolationColor;
        float                   interpolationColorTime;

        core::InterpolationType interpolationSize;
        float                   interpolationSizeTime;
    };

    struct Skin
    {
    public:
        Skin();
        ~Skin();

        std::string name;

        std::shared_ptr<render::Font> font;

        float text_size;

        std::string button_image;
        ButtonState button_idle;
        ButtonState button_hot;
        ButtonState button_active_hot;
    };
}  // namespace euphoria::gui

#endif  // GUI_SKIN_H
