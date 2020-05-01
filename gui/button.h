#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "core/rect.h"
#include "core/rgb.h"
#include "core/vec2.h"
#include "core/interpolate.h"
#include "gui/widget.h"
#include "gui/textdata.h"

#include <memory>

namespace euphoria::render
{
    struct ScalableSprite;
}

namespace euphoria::gui
{
    struct Skin;
    struct ButtonState;

    struct Button : public Widget
    {
        Button(UiState* state);

        ~Button();

        virtual void
        OnClicked() = 0;

        void
        Step(float dt) override;

        core::Sizef
        CalculateMinimumSize() const override;

        void
        Render(render::SpriteRenderer* renderer) const override;

        void
        Visit(Visitor* visitor) override;

        TextData&
        Text();

        void
        SetSprite(std::shared_ptr<render::ScalableSprite> sprite);

        void
        OnSize() override;

        void
        SetSkin(Skin* skin);

        ButtonState* last_state_;
        std::shared_ptr<render::ScalableSprite> sprite_;
        TextData text_;
        Skin* skin_;

        core::Interpolate<float, core::FloatTransform> scale_;
        core::Interpolate<core::Rgb, core::RgbTransform> image_color_;
        core::Interpolate<core::Rgb, core::RgbTransform> text_color_;
        core::Interpolate<core::vec2f, core::Vec2fTransform> position_displacement_;
    };
}

#endif  // GUI_BUTTON_H

