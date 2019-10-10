#ifndef GUI_LAYOUT_H
#define GUI_LAYOUT_H

#include "core/rect.h"
#include "core/size.h"

#include <vector>
#include <memory>

namespace euphoria::gui
{
    struct Widget;

    struct Layout
    {
    public:
        Layout();

        virtual ~Layout();

        virtual core::Sizef
        CalculateMinimumArea(
                const std::vector<std::shared_ptr<Widget>>& widgets) const = 0;

        virtual void
        DoLayout(
                std::vector<std::shared_ptr<Widget>>* widgets,
                const core::Rectf&                    area) const = 0;
    };

    struct TableLayout : public Layout
    {
    public:
        TableLayout(
                const std::vector<bool> expandable_rows,
                const std::vector<bool> expandable_cols,
                float                   combined_padding);

        virtual core::Sizef
        CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>&
                                     widgets) const override;

        virtual void
        DoLayout(
                std::vector<std::shared_ptr<Widget>>* widgets,
                const core::Rectf&                    area) const override;

        std::vector<bool> expandable_rows_;
        std::vector<bool> expandable_cols_;
        float             combined_padding_;
    };

    struct SingleRowLayout : public Layout
    {
    public:
        SingleRowLayout(float padding);

        virtual core::Sizef
        CalculateMinimumArea(const std::vector<std::shared_ptr<Widget>>&
                                     widgets) const override;

        virtual void
        DoLayout(
                std::vector<std::shared_ptr<Widget>>* widgets,
                const core::Rectf&                    area) const override;

    private:
        float padding_;
    };
}  // namespace euphoria::gui

#endif  // GUI_LAYOUT_H
