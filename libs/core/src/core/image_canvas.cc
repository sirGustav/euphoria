#include "core/image_canvas.h"

#include "core/image.h"
#include "core/assert.h"
#include "core/numeric.h"
#include "core/mat2.h"
#include "core/image_draw.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

#include "core/log.h"


namespace euphoria::core
{
    vec2f
    canvas::transform_position(const vec2f& v) const
    {
        const auto vv = transform * vec3f {v, 1};
        return vec2f {vv.x, static_cast<float>(target_image->height) - vv.y};
    }

    canvas::canvas(image* i)
        : fill_style(color::black)
        , target_image(i)
        , transform(mat3f::identity())
        , building_path(false)
    {
    }

    void
    canvas::fill_rect(int x, int y, int w, int h) const
    {
        ASSERTX(w > 0, w);
        ASSERTX(h > 0, h);
        draw_rect(
                target_image,
                fill_style,
                recti::from_top_left_width_height(vec2i{x, target_image->height - y}, w, h));
    }

    void
    canvas::translate(float x, float y)
    {
        const auto m = mat3f::from_translation2d(vec2f {x, y});
        transform = transform * m;
    }

    void
    canvas::rotate(float r)
    {
        transform = transform * mat3f{mat2f::from_rotation(angle::from_radians(r))};
    }

    void
    canvas::begin_path()
    {
        ASSERT(!building_path);
        path.resize(0);
        building_path = true;
    }

    void
    canvas::close_path()
    {
        ASSERT(building_path);
        building_path = false;
    }

    void
    canvas::move_to(float x, float y)
    {
        ASSERT(building_path);
        ASSERT(path.empty());
        path.push_back(transform_position(vec2f(x, y)));
    }

    void
    canvas::line_to(float dx, float dy)
    {
        ASSERT(building_path);
        if(path.empty())
        {
            path.push_back(transform_position(vec2f::zero()));
        }
        path.push_back(transform_position(vec2f(dx, dy)));
    }

    void
    canvas::fill() const
    {
        ASSERT(!building_path);
        fill_poly(target_image, fill_style, path);
    }
}

