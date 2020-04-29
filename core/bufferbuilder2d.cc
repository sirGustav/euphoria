#include "core/bufferbuilder2d.h"

#include "core/vec3.h"


namespace euphoria::core
{
    Point::Point(float x, float y, float u, float v)
        : pos(x, y)
        , vert(u, v)
    {
    }


    Point::Point(const vec2f& apos, const vec2f& avert)
        : pos(apos)
        , vert(avert)
    {
    }


    BufferBuilder2d::BufferBuilder2d() = default;


    void
    BufferBuilder2d::AddVertex(const Point& p)
    {
        data.push_back(p.pos.x);
        data.push_back(p.pos.y);
        data.push_back(p.vert.x);
        data.push_back(p.vert.y);
    }


    namespace
    {
        bool
        IsCcw
        (
            const std::vector<float>& data,
            unsigned int a,
            unsigned int b,
            unsigned int c
        )
        {
            const auto va = vec3f{data[a*4], data[a*4+1], 0};
            const auto vb = vec3f{data[b*4], data[b*4+1], 0};
            const auto vc = vec3f{data[c*4], data[c*4+1], 0};
            const auto cr = cross
            (
                vec3f::FromTo(va, vb),
                vec3f::FromTo(va, vc)
            );
            return cr.z < 0;
        }
    }


    void
    BufferBuilder2d::AddTriangle
    (
        unsigned int a,
        unsigned int b,
        unsigned int c
    )
    {
        ASSERTX
        (
            a < (data.size() / 4) &&
            b < (data.size() / 4) &&
            c < (data.size() / 4),
            a, b, c, data.size()/4
        );
        ASSERTX(IsCcw(data, a, b, c), a, b, c);
        tris.push_back(a);
        tris.push_back(b);
        tris.push_back(c);
    }


    void
    BufferBuilder2d::AddQuad
    (
        const Point& a,
        const Point& b,
        const Point& c,
        const Point& d
    )
    {
        const unsigned int ai = data.size();
        const unsigned int bi = ai + 1;
        const unsigned int ci = ai + 2;
        const unsigned int di = ai + 3;

        AddVertex(a);
        AddVertex(b);
        AddVertex(c);
        AddVertex(d);

        AddTriangle(ci, bi, ai);
        AddTriangle(ci, di, bi);
    }
}

