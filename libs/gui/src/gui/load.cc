#include "gui/load.h"

#include "core/proto.h"
#include "core/log.h"

#include "render/texturecache.h"
#include "render/scalablesprite.h"
#include "render/fontcache.h"

#include "gui/layoutcontainer.h"
#include "gui/layout.h"
#include "gui/button.h"
#include "gui/panelwidget.h"
#include "gui/skin.h"
#include "gui/root.h"

#include "gaf_gui.h"

#include <iostream>
#include <map>

namespace euphoria::gui
{
    std::shared_ptr<layout>
    create_layout(const ::gui::Layout& c)
    {
        if(c.table)
        {
            LOG_INFO("Creating a table layout");
            return create_table_layout
            (
                c.table->expanded_rows,
                c.table->expanded_cols,
                c.table->padding
            );
        }
        else if(c.single_row)
        {
            LOG_INFO("Creating a single row layout");
            return create_single_row_layout(c.single_row->padding);
        }
        else
        {
            LOG_ERROR("Missing a layout");
            return create_single_row_layout(0);
        }
    }


    struct command_button : public button
    {
        explicit command_button(gui::ui_state* state)
            : button(state)
        {
        }

        void
        on_clicked() override
        {
            // todo(Gustav): set up command to actually do something
            LOG_INFO("Executing cmd: {0}", cmd);
        }

        std::string cmd;
    };


    void
    build_layout_container
    (
        core::vfs::file_system* fs,
        ui_state* state,
        layout_container* root,
        const ::gui::LayoutContainer& c,
        render::texture_cache* cache,
        const std::map<std::string, skin*>& skins
    );


    void
    setup_layout(layout_data* data, const ::gui::Widget& src)
    {
        data->column = src.column;
        data->row = src.row;
        data->preferred_width = src.preferred_width;
        data->preferred_height = src.preferred_height;
    }


    lrtb
    lrtb_from_gaf(const ::gui::Lrtb& lrtd)
    {
        lrtb r;
        r.left = lrtd.left;
        r.right = lrtd.right;
        r.top = lrtd.top;
        r.bottom = lrtd.bottom;
        return r;
    }


    std::shared_ptr<widget>
    create_widget
    (
        core::vfs::file_system* fs,
        ui_state* state,
        const ::gui::Widget& w,
        render::texture_cache* cache,
        const std::map<std::string, skin*>& skins
    )
    {
        std::shared_ptr<widget> ret;

        if(w.button)
        {
            LOG_INFO("Creating a button widget");
            auto b = std::make_shared<command_button>(state);

            const std::string skin_name = w.button->skin;
            const auto skin_it = skins.find(skin_name);
            skin* skin = nullptr;

            if(skin_it == skins.end())
            {
                LOG_ERROR("Failed to find skin '{0}'", skin_name);
            }
            else
            {
                b->set_skin(skin_it->second);

                skin = skin_it->second;

                if(skin_it->second->button_image.has_value())
                {
                    std::shared_ptr<render::scalable_sprite> sp
                    {
                        new render::scalable_sprite
                        {
                            fs,
                            skin->button_image.value(),
                            cache
                        }
                    };
                    b->sprite = sp;
                }
            }
            ret = b;
            b->cmd = w.button->command;
            b->text.update_string(w.button->text);

            if(skin != nullptr)
            {
                b->text.set_font(skin->font);
            }
        }
        else if(w.panel)
        {
            LOG_INFO("Creating a panel widget");
            auto l = std::make_shared<panel_widget>(state);
            ret = l;
            build_layout_container
            (
                fs,
                state,
                &l->container,
                w.panel->container,
                cache,
                skins
            );
        }
        else
        {
            LOG_ERROR("Invalid widget");
        }

        ASSERT(ret);

        // load basic widget data
        ret->name = w.name;
        ret->padding = lrtb_from_gaf(w.padding);
        ret->margin = lrtb_from_gaf(w.margin);

        setup_layout(&ret->layout, w);

        return ret;
    }

    void
    build_layout_container
    (
        core::vfs::file_system* fs,
        ui_state* state,
        layout_container* root,
        const ::gui::LayoutContainer& c,
        render::texture_cache* cache,
        const std::map<std::string, skin*>& skins
    )
    {
        root->layout = create_layout(c.layout);
        for(const auto& widget: c.widgets)
        {
            root->add(create_widget(fs, state, widget, cache, skins));
        }
    }

    core::rgb
    load(const ::gui::Rgb& src)
    {
        return core::rgb(src.r, src.g, src.b);
    }

    core::InterpolationType
    load(::gui::InterpolationType t)
    {
        switch(t)
        {
#define FUN(NAME, FUNC) \
    case ::gui::InterpolationType::NAME: return core::InterpolationType::NAME;
            // Linear interpolation (no easing)
            FUN(Linear, LinearInterpolation)

            // Quadratic easing; p^2
            FUN(QuadIn, QuadraticEaseIn)
            FUN(QuadOut, QuadraticEaseOut)
            FUN(Quad, QuadraticEaseInOut)

            // Cubic easing; p^3
            FUN(CubicIn, CubicEaseIn)
            FUN(CubicOut, CubicEaseOut)
            FUN(Cubic, CubicEaseInOut)

            // Quartic easing; p^4
            FUN(QuartIn, QuarticEaseIn)
            FUN(QuartOut, QuarticEaseOut)
            FUN(Quart, QuarticEaseInOut)

            // Quintic easing; p^5
            FUN(QuintIn, QuinticEaseIn)
            FUN(QuintOut, QuinticEaseOut)
            FUN(Quint, QuinticEaseInOut)

            // Sine wave easing; sin(p * PI/2)
            FUN(SineIn, SineEaseIn)
            FUN(SineOut, SineEaseOut)
            FUN(Sine, SineEaseInOut)

            // Circular easing; sqrt(1 - p^2)
            FUN(CircIn, CircularEaseIn)
            FUN(CircOut, CircularEaseOut)
            FUN(Circ, CircularEaseInOut)

            // Exponential easing, base 2
            FUN(ExpIn, ExponentialEaseIn)
            FUN(ExpOut, ExponentialEaseOut)
            FUN(Exp, ExponentialEaseInOut)

            // Exponentially-damped sine wave easing
            FUN(ElasticIn, ElasticEaseIn)
            FUN(ElasticOut, ElasticEaseOut)
            FUN(Elastic, ElasticEaseInOut)

            // Overshooting cubic easing;
            FUN(BackIn, BackEaseIn)
            FUN(BackOut, BackEaseOut)
            FUN(Back, BackEaseInOut)

            // Exponentially-decaying bounce easing
            FUN(BounceIn, BounceEaseIn)
            FUN(BounceOut, BounceEaseOut)
            FUN(Bounce, BounceEaseInOut)
#undef FUN
        default: return core::InterpolationType::INVALID;
        }
    }


    button_state
    load_button(const ::gui::ButtonState& src)
    {
        button_state ret;
        // ret.image = src.image();
        ret.scale = src.scale;
        ret.image_color = load(src.image_color);
        ret.text_color = load(src.text_color);
        ret.dx = src.dx;
        ret.dy = src.dy;
        ret.interpolation_color =
        {
                load(src.interpolate_color),
                src.interpolate_color_time
        };
        ret.interpolation_size =
        {
                load(src.interpolate_size),
                src.interpolate_size_time
        };
        ret.interpolation_position =
        {
                load(src.interpolate_position),
                src.interpolate_position_time
        };
        return ret;
    }


    std::shared_ptr<skin>
    load_skin(const ::gui::Skin& src, render::font_cache* font)
    {
        std::shared_ptr<skin> skin(new gui::skin());
        skin->name = src.name;
        skin->font = font->get_font
        (
            core::vfs::file_path::from_script(src.font).value_or
            (
                core::vfs::file_path{"~/invalid_font_file"}
            )
        );
        skin->button_image = core::vfs::file_path::from_script_or_empty
        (
            src.button_image
        );
        skin->text_size = src.text_size;
        skin->button_idle = load_button(src.button_idle);
        skin->button_hot = load_button(src.button_hot);
        skin->button_active_hot = load_button(src.button_active_hot);
        return skin;
    }


    bool
    load_gui
    (
        root* root,
        core::vfs::file_system* fs,
        render::font_cache* font,
        const core::vfs::file_path& path,
        render::texture_cache* cache
    )
    {
        ::gui::File f;
        const std::string load_result = core::read_json_to_gaf_struct_or_get_error_message(fs, &f, path);
        if(false == load_result.empty())
        {
            LOG_ERROR("Failed to load gui from {0}: {1}", path, load_result);
            return false;
        }

        root->cursor_image = cache->get_texture
        (
            core::vfs::file_path::from_script_or_empty(f.cursor_image)
        );
        root->hover_image = cache->get_texture
        (
            core::vfs::file_path::from_script_or_empty(f.hover_image)
        );

        std::map<std::string, skin*> skin_map;

        for(const auto& skin: f.skins)
        {
            std::shared_ptr<gui::skin> skin_ptr = load_skin(skin, font);
            skin_map.insert(std::make_pair(skin.name, skin_ptr.get()));
            root->skins.push_back(skin_ptr);
        }

        build_layout_container
        (
            fs,
            &root->state,
            &root->container,
            f.root,
            cache,
            skin_map
        );

        return root->container.has_any_widgets();
    }
}
