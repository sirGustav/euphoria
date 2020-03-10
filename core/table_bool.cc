#include "core/table_bool.h"

#include "core/image_draw.h"
#include "core/assert.h"

namespace euphoria::core
{
    void
    SetWhiteNoise
    (
        BoolTable* world,
        Fourway<BorderSetupRule> border_control,
        std::function<bool()> rng
    )
    {
        world->SetAll([&](int x, int y)
        {
            const auto is_on_left_border = x == 0;
            const auto is_on_right_border = x == world->GetWidth() - 1;
            const auto is_on_up_border = y == 0;
            const auto is_on_down_border = y == world->GetHeight() - 1;

#define CHECK_BORDER(b, p) \
            do \
            { \
                if( b && border_control.p != BorderSetupRule::Random ) \
                { return border_control.p == BorderSetupRule::AlwaysWall; } \
            } while(false)

            CHECK_BORDER(is_on_left_border, left);
            CHECK_BORDER(is_on_right_border, right);
            CHECK_BORDER(is_on_up_border, up);
            CHECK_BORDER(is_on_down_border, down);
#undef CHECK_BORDER

            return rng();
        });
    }


    int
    CountWalls
    (
        const BoolTable& world,
        Fourway<OutsideRule> outside_rule,
        int cx,
        int cy,
        int step
    )
    {
        int walls = 0;
        for (int y = cy - step; y <= cy + step; y += 1)
        {
            for (int x = cx - step; x <= cx + step; x += 1)
            {
                if (x == cx && y == cy)
                {
                    // self is not a wall
                    continue;
                }

                auto contains = [&world](int x, int y)
                {
                    return world.Indices().ContainsInclusive(x, y);
                };

                if (contains(x, y))
                {
                    // it is inside
                    if (world(x, y))
                    {
                        walls += 1;
                    }
                }
                else
                {
                    // todo(Gustav): make this cleaner
                    bool x_count_handled = false;
                    auto nx = x;

                    if (!contains(x, 0))
                    {
                        // x is outside
                        const auto xrule = x < 0 ? outside_rule.left : outside_rule.right;
                        switch (xrule)
                        {
                        case OutsideRule::Wall:
                            walls += 1;
                            x_count_handled = true;
                            break;
                        case OutsideRule::Empty:
                            // pass
                            x_count_handled = true;
                            break;
                        case OutsideRule::Mirror:
                            // todo: implement this!
                            nx = KeepWithin(world.Indices().GetXRange(), x);
                            break;
                        case OutsideRule::Wrap:
                            nx = Wrap(world.Indices().GetXRange(), x);
                            break;
                        default:
                            DIE("Unhandled case");
                            break;
                        }
                    }

                    if (x_count_handled == false)
                    {
                        bool y_count_handled = false;
                        auto ny = y;

                        if (!contains(0, y))
                        {
                            // y is outside
                            const auto yrule = y < 0 ? outside_rule.up : outside_rule.down;
                            switch (yrule)
                            {
                            case OutsideRule::Wall:
                                walls += 1;
                                y_count_handled = true;
                                break;
                            case OutsideRule::Empty:
                                // pass
                                y_count_handled = true;
                                break;
                            case OutsideRule::Mirror:
                                // todo: implement this!
                                ny = KeepWithin(world.Indices().GetYRange(), y);
                                y_count_handled = false;
                                break;
                            case OutsideRule::Wrap:
                                ny = Wrap(world.Indices().GetYRange(), y);
                                y_count_handled = false;
                                break;
                            default:
                                DIE("Unhandled case");
                                break;
                            }
                        }

                        if (y_count_handled == false)
                        {
                            if (world(nx, ny))
                            {
                                walls += 1;
                            }
                        }
                    }
                }
            }
        }
        return walls;
    }


    Wallcounter::Wallcounter
    (
        const BoolTable& w,
        Fourway<OutsideRule> r,
        int x,
        int y
    )
        : world(w)
        , outside_rule(r)
        , cx(x)
        , cy(y)
    {
    }

    int
    Wallcounter::Count(int step) const
    {
        return CountWalls(world, outside_rule, cx, cy, step);
    }


    void
    SmoothMap
    (
        BoolTable* world,
        Fourway<OutsideRule> outside_rule,
        std::function<std::optional<bool>(const Wallcounter&)> smooth_function
    )
    {
        const BoolTable current = *world;

        world->SetAll([&current, outside_rule, smooth_function](int x, int y)
        {
            const auto walls = Wallcounter{ current, outside_rule, x, y };
            const auto smoothed_wall = smooth_function(walls);
            return smoothed_wall.value_or(current(x, y));
        });
    }


    Image
    Draw
    (
        const BoolTable& world,
        Rgbi wall_color,
        Rgbi space_color,
        int scale
    )
    {
        Image image;
        image.SetupNoAlphaSupport(
            world.GetWidth() * scale, world.GetHeight() * scale);

        Clear(&image, space_color);

        for (int x = 0; x < world.GetWidth(); x += 1)
        {
            for (int y = 0; y < world.GetHeight(); y += 1)
            {
                const auto px = x * scale;
                const auto py = y * scale;
                const auto color = world(x, y) ? wall_color : space_color;
                DrawSquare(&image, color, px, py + scale - 1, scale);
            }
        }

        return image;
    }

    BEGIN_ENUM_LIST(BorderSetupRule)
        ENUM_VALUE(BorderSetupRule, AlwaysWall)
        ENUM_VALUE(BorderSetupRule, AlwaysEmpty)
        ENUM_VALUE(BorderSetupRule, Random)
    END_ENUM_LIST()

    BEGIN_ENUM_LIST(OutsideRule)
        ENUM_VALUE(OutsideRule, Wall)
        ENUM_VALUE(OutsideRule, Empty)
        ENUM_VALUE(OutsideRule, Mirror)
        ENUM_VALUE(OutsideRule, Wrap)
    END_ENUM_LIST()
}