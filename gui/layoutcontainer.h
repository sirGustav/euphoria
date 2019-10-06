#ifndef GUI_LAYOUTCONTAINER_H
#define GUI_LAYOUTCONTAINER_H

#include "gui/container.h"
#include "core/rect.h"
#include "core/size.h"

#include <memory>
#include <vector>


namespace euphoria::gui
{
    class Layout;

    std::shared_ptr<Layout>
    CreateTableLayout(
            const std::vector<bool> expandable_rows,
            const std::vector<bool> expandable_cols,
            float                   combined_padding);

    std::shared_ptr<Layout>
    CreateSingleRowLayout(float padding);

    class LayoutContainer : public Container
    {
        public:
        LayoutContainer();
        virtual ~LayoutContainer();

        void
        SetLayout(std::shared_ptr<Layout> layout);
        std::shared_ptr<Layout>
        GetLayout();

        void
        DoLayout(core::Rectf area);

        core::Sizef
        CalculateMinimumArea() const;

        private:
        std::shared_ptr<Layout> layout_;
    };
}  // namespace euphoria::gui

#endif  // GUI_LAYOUTCONTAINER_H
