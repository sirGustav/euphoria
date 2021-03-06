#pragma once


#include "core/vec3.h"

namespace euphoria::core
{
    struct random;

    struct aabb
    {
        aabb(const vec3f& amin, const vec3f& amax);


        [[nodiscard]] vec3f
        wrap(const vec3f& vec) const;


        void
        extend(const vec3f& vec);


        void
        extend(const aabb& aabb);


        [[nodiscard]] static aabb
        create_empty();


        [[nodiscard]] vec3f
        get_size() const;


        [[nodiscard]] bool
        is_valid() const;


        [[nodiscard]] vec3f
        get_offset() const;


        void
        offset(const vec3f& vec);


        [[nodiscard]] aabb
        offset_copy(const vec3f& vec) const;


        [[nodiscard]] vec3f
        get_random_point(random* rand) const;


        vec3f min;
        vec3f max;
    };

    template <typename Stream>
    Stream&
    operator<<(Stream& s, const aabb& a)
    {
        s << "{" << a.min << ", " << a.max << "}";
        return s;
    }

}
