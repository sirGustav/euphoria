#include "render/spritebatch.h"
#include "render/gl.h"

#include "core/assert.h"

namespace euphoria::render
{
    // vertex + uv + color
    const unsigned int QuadCount = 10;
    const unsigned int Stride    = 2 * 4 + 2 * 4 + 4 * 4;

    SpriteBatch::SpriteBatch() : inside_(false), count_(0), rendercalls_(0)
    {
        // the static casts are weird, but they stop clang-tidy from complaining about
        // 'misplaced widening casts'
        data_.reserve(static_cast<size_t>(Stride) * QuadCount);
        index_.reserve(static_cast<size_t>(6) * QuadCount);
    }

    SpriteBatch::~SpriteBatch() = default;

    void
    SpriteBatch::Begin()
    {
        ASSERT(!inside_ && "Already open, missing call to end.");
        rendercalls_ = 0;
    }

    void
    SpriteBatch::Quad(
            const core::vec2f&   pos,
            const core::Sizef&   quad,
            const core::Rectf&   uv,
            const core::scale2f& center,
            const core::Angle&   rotation,
            const core::Rgba&    color)
    {
        ASSERT(inside_ && "batch need to be open");

        if((count_ + 1) >= QuadCount)
        {
            Flush();
        }

        // add vertices
        const float w           = quad.width;
        const float h           = quad.height;
        const float sy          = -Sin(rotation);
        const float left        = pos.x + (-center.x * w) * sy;
        const float right       = pos.x + (-center.x * w + w) * sy;
        const float top         = pos.y + (-center.y * h) * sy;
        const float bottom      = pos.y + (-center.y * h + h) * sy;
        const auto  upper_left  = core::vec2f(left, top);
        const auto  upper_right = core::vec2f(right, top);
        const auto  lower_left  = core::vec2f(left, bottom);
        const auto  lower_right = core::vec2f(right, bottom);

        data_.push_back(upper_left.x);
        data_.push_back(upper_left.y);

        data_.push_back(upper_right.x);
        data_.push_back(upper_right.y);

        data_.push_back(lower_right.x);
        data_.push_back(lower_right.y);

        data_.push_back(lower_left.x);
        data_.push_back(lower_left.y);

        // add uv coordinate
        data_.push_back(uv.left);
        data_.push_back(uv.top);
        data_.push_back(uv.right);
        data_.push_back(uv.top);
        data_.push_back(uv.right);
        data_.push_back(uv.bottom);
        data_.push_back(uv.left);
        data_.push_back(uv.bottom);

        // add color
        for(int i = 0; i < 4; ++i)
        {
            data_.push_back(color.r);
            data_.push_back(color.g);
            data_.push_back(color.b);
            data_.push_back(color.a);
        }

        // add index
        int start = count_ * 4;

        index_.push_back(start + 0);
        index_.push_back(start + 1);
        index_.push_back(start + 2);

        index_.push_back(start + 0);
        index_.push_back(start + 2);
        index_.push_back(start + 3);

        count_ += 1;
    }

    void
    SpriteBatch::End()
    {
        ASSERT(inside_ && "not open, missing begin.");
        Flush();
        inside_ = false;
    }

    void
    SpriteBatch::Flush()
    {
        if(count_ == 0)
        {
            return;
        }
        // todo: build vbo & render vbo
        // remove all items
        count_ = 0;
        data_.resize(0);
        rendercalls_ += 1;
    }

}  // namespace euphoria::render