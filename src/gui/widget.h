#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include <string>

#include "core/rect.h"
#include "core/size.h"

#include "gui/layoutdata.h"

namespace euphoria::render
{
    struct SpriteRenderer;
}

namespace euphoria::gui
{
    struct UiState;
    struct Visitor;


    struct Lrtb
    {
        float left;
        float right;
        float top;
        float bottom;
    };


    struct Widget
    {
        std::string name;
        Lrtb margin;
        Lrtb padding;

        explicit
        Widget(UiState* state);

        virtual
        ~Widget();

        bool
        IsActive() const;

        bool
        IsHot() const;

        virtual void
        Step(float dt) = 0;

        virtual void
        OnSize();

        core::Rectf
        GetClientRect() const;

        core::Rectf
        GetBackgroundRect() const;

        void
        SetRect(const core::Rectf& r);

        core::Sizef
        GetPreferredSize() const;

        virtual core::Sizef
        CalculateMinimumSize() const = 0;

        virtual void
        Render(render::SpriteRenderer* renderer) const = 0;

        virtual
        void
        Visit(Visitor* visitor) = 0;

    // todo(Gustav): public/private/public/private nonsense?
    protected:
        const UiState&
        GetState() const;

        UiState*
        GetStatePtr();

    private:
        UiState* state_;

    public:
        LayoutData layout;
        core::Rectf rect_;
    };
}

#endif  // GUI_WIDGET_H
