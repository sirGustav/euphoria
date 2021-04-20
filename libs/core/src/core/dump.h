#ifndef CORE_DUMP_H
#define CORE_DUMP_H

#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <fstream>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/colors.h"

namespace euphoria::core
{
    struct plane;
    struct ray3f;
}

namespace euphoria::core::dump2d
{
    namespace strokes
    {
        std::vector<int> Dash(int size);
    }

    struct Poly
    {
        rgbi stroke_color = Color::Black;
        std::optional<rgbi> fill_color;
        bool is_closed = false;
        std::vector<int> stroke;

        Poly& Stroke(const std::vector<int>& new_stroke);

        Poly& Close();
        Poly& Fill(const rgbi& fill_color);

        std::vector<vec2f> points;
    };

    struct Text
    {
        vec2f point;
        std::string text;
        rgbi color;

        Text(const vec2f& p, const std::string& t, const rgbi& c = Color::Black);
    };

    struct Circle
    {
        vec2f point;
        float radius;
        std::optional<rgbi> line_color;
        std::optional<rgbi> fill_color;

        Circle& Line(const rgbi& lc);

        Circle(const vec2f& p, float r, std::optional<rgbi> c = std::nullopt);
    };

    struct Group;

    // todo(Gustav): replace with std::variant
    struct Item
    {
        explicit Item(const Poly& p);
        explicit Item(const Text& p);
        explicit Item(const Group& g);
        explicit Item(const Circle& c);

        std::shared_ptr<Poly>  poly;
        std::shared_ptr<Text>  text;
        std::shared_ptr<Group> group;
        std::shared_ptr<Circle> circle;
    };
    const Poly*  AsPoly(const Item* item);
    const Text*  AsText(const Item* item);
    const Group* AsGroup(const Item* item);
    const Circle* AsCircle(const Item* item);

    template<typename TBase>
    struct AddWrapper
    {
        template<typename TItem>
        TBase& operator<<(const TItem& item)
        {
            return static_cast<TBase*>(this)->Add(Item{item});
        }
    };

    struct Group : public AddWrapper<Group>
    {
        std::vector<Item> items;

        Group& Add(const Item& item);
    };

    struct Dumper : AddWrapper<Dumper>
    {
        rgbi canvas_color = Color::White;
        std::vector<Item> items;

        bool add_axis = false;
        int point_size = -1;
        bool point_text = false;

        float gridx =-1;
        float gridy =-1;

        Dumper& AddAxis();

        Dumper& Grid(float xy);

        Dumper& DrawPoints(int size=3);

        Dumper& Add(const Item& item);

        // calculate total area size and offset so that x+offset will never be lower than 0
        [[nodiscard]] std::pair<vec2f,vec2f> CalculateSizeAndOffset() const;

        void Write(const std::string& path, int width=1280, int height=1024, int space = 6) const;
    };
}

namespace euphoria::core::dump3d
{
    struct Dumper
    {
        explicit Dumper(const std::string& path);
        ~Dumper();

        Dumper(const Dumper&) = delete;
        Dumper(Dumper&&) = delete;
        void operator=(const Dumper&) = delete;
        void operator=(Dumper&&) = delete;

        void AddSphere(const vec3f& p, float radius, const rgbi& color);
        void AddLines(const std::vector<vec3f>& lines, const rgbi& color);
        void AddPlane(const plane& plane, const rgbi& color);
        void AddArrow(const ray3f& ray, const rgbi& color);

        void AddAxis();
        void AddGrid();


        std::ofstream file;
    };
}

#endif  // CORE_DUMP_H
