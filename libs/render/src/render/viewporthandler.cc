#include "render/viewporthandler.h"

#include "core/viewportdef.h"
#include "core/cint.h"

#include "render/viewport.h"
#include "render/init.h"
#include "render/shader.h"


namespace
{
    namespace core = euphoria::core;

    core::ViewportDef
    CreateDefinition(const euphoria::render::ViewportHandler& handler)
    {
        switch(handler.type)
        {
        case euphoria::render::ViewportType::FitWithBlackBars:
            return core::ViewportDef::FitWithBlackBars
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::ViewportType::ScreenPixel:
            return core::ViewportDef::ScreenPixel
            (
                handler.window_width,
                handler.window_height
            );
        case euphoria::render::ViewportType::Extend:
            return core::ViewportDef::Extend
            (
                handler.virtual_width,
                handler.virtual_height,
                handler.window_width,
                handler.window_height
            );
        default:
            DIE("Unhandled viewport case");
            return core::ViewportDef::ScreenPixel
            (
                handler.window_width,
                handler.window_height
            );
        }
    }


    void Apply
    (
        euphoria::render::ViewportHandler* handler,
        const core::ViewportDef& vp,
        bool shaders_too
    )
    {
        if(shaders_too)
        {
            const auto projection = handler->init->GetOrthoProjection
            (
                vp.virtual_width,
                vp.virtual_height
            );
            for(auto* shader: handler->shaders)
            {
                shader->SetUniform
                (
                    shader->GetUniform("projection"),
                    projection
                );
            }
        }

        if(handler->virtual_screen != nullptr)
        {
            *handler->virtual_screen = core::Rectf::FromWidthHeight
            (
                vp.virtual_width,
                vp.virtual_height
            );
        }

        auto viewport = euphoria::render::Viewport{vp.screen_rect};
        viewport.Activate();
    }
}


namespace euphoria::render
{
    ViewportHandler::ViewportHandler(Init* i, core::Rectf* s)
        : init(i)
        , virtual_screen(s)
    {
    }


    void
    ViewportHandler::Add(Shader* shader)
    {
        shaders.emplace_back(shader);
    }


    void
    ViewportHandler::SetSize(int width, int height)
    {
        window_width = width;
        window_height = height;

        const auto vp = CreateDefinition(*this);

        Apply(this, vp, true);
    }


    void
    ViewportHandler::ClearBlack()
    {
        if(type != ViewportType::FitWithBlackBars) { return; }

        Apply
        (
            this,
            core::ViewportDef::ScreenPixel(window_width, window_height),
            false
        );

        init->ClearScreen(core::color::black);

        Apply
        (
            this,
            CreateDefinition(*this),
            false
        );
    }



    Viewport
    ViewportHandler::GetFullViewport() const
    {
        const auto viewport = Viewport
        {
            core::Recti::FromWidthHeight(window_width, window_height)
                .SetBottomLeftToCopy
                (
                    0,
                    0
                )
        };
        return viewport;
    }
}

