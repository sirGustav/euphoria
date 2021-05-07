#include "gui/widget.h"

#include "core/log.h"

#include "gui/uistate.h"


namespace euphoria::gui
{
    Widget::Widget(UiState* state)
        : state_(state)
    {
    }


    Widget::~Widget() = default;


    bool
    Widget::IsActive() const
    {
        return state_->active == this;
    }


    bool
    Widget::IsHot() const
    {
        return state_->hot == this;
    }


    void
    Widget::OnSize()
    {
    }


    core::rectf
    Widget::GetClientRect() const
    {
        return rect_.inset_copy(
                padding.left, padding.right, padding.top, padding.bottom);
    }


    core::rectf
    Widget::GetBackgroundRect() const
    {
        return rect_;
    }


    void
    Widget::SetRect(const core::rectf& r)
    {
        rect_ = r.inset_copy
        (
            margin.left,
            margin.right,
            margin.top,
            margin.bottom
        );
        LOG_INFO("Setting gui rect of '{0}' {1}", name, rect_);
        OnSize();
    }


    core::Sizef
    Widget::GetPreferredSize() const
    {
        const auto min = this->CalculateMinimumSize();
        const auto padding_width = padding.left + padding.right;
        const auto padding_height = padding.top + padding.bottom;
        const auto margin_width = margin.left + margin.right;
        const auto margin_height = margin.top + margin.bottom;

        return core::Sizef::FromWidthHeight
        (
            core::max
            (
                min.width + padding_width + margin_width,
                layout.GetPreferredWidth()
            ),
            core::max
            (
                min.height + padding_height + margin_height,
                layout.GetPreferredHeight()
            )
        );
    }


    const UiState&
    Widget::GetState() const
    {
        return *state_;
    }


    UiState*
    Widget::GetStatePtr() const
    {
        return state_;
    }
}

