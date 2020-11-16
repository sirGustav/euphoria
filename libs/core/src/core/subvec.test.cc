#include "core/subvec.h"

#include "core/str.h"

#include "tests/utils.h"
#include "tests/vectorequals.h"

using namespace euphoria::core;
using namespace euphoria::tests;


namespace
{
    FalseString
    Check
    (
        const std::vector<int>& lhs,
        const std::vector<int>& rhs
    )
    {
        return euphoria::tests::VectorEquals
        (
            lhs,
            rhs,
            [](int m) -> std::string
            {
                return Str{} << m;
            },
            [](int lhs, int rhs) -> FalseString
            {
                if(lhs == rhs) { return FalseString::True(); }
                else
                {
                    return FalseString::False(Str() << lhs << " != " << rhs);
                }
            }
        );
    }
}


TEST_CASE("subec", "[subvec]")
{
    SECTION("default")
    {
        auto vec = std::vector<int>{1, 2, 3, 4, 5, 6};
        auto sub = SubVec{&vec};

        CHECK_FALSE(vec.empty());
        CHECK(sub.size() == 6);
        CHECK(sub[0] == 1);
        CHECK(sub[2] == 3);
        CHECK(Check(vec, {1, 2, 3, 4, 5, 6}));
    }

    SECTION("empty")
    {
        auto vec = std::vector<int>{};
        auto sub = SubVec{&vec};

        CHECK(vec.empty());
        CHECK(sub.size() == 0);
        CHECK(Check(vec, {}));
    }

    SECTION("sort")
    {
        auto vec = std::vector<int>{6, 5, 4, 3, 2, 1};
        auto sub = SubVec{&vec};

        CHECK(sub[0] == 6);
        CHECK(sub[2] == 4);

        std::sort(sub.begin(), sub.end());

        CHECK_FALSE(vec.empty());
        CHECK(sub.size() == 6);
        CHECK(Check(vec, {1, 2, 3, 4, 5, 6}));
    }

    SECTION("sub left")
    {
        auto vec = std::vector<int>{3, 2, 1, 6, 5, 4};
        auto sub = SubVec{&vec};

        CHECK(sub[0] == 3);
        CHECK(sub[2] == 1);

        auto left = sub.Sub(0, 3);
        auto right = sub.Sub(3, 6);

        std::sort(left.begin(), left.end());
        // std::sort(right.begin(), right.end());

        CHECK_FALSE(left.empty());
        CHECK_FALSE(right.empty());

        CHECK(left.size() == 3);
        CHECK(right.size() == 3);

        CHECK_FALSE(vec.empty());
        CHECK(sub.size() == 6);
        CHECK(Check(vec, {1, 2, 3, 6, 5, 4}));
    }

    SECTION("sub right")
    {
        auto vec = std::vector<int>{3, 2, 1, 6, 5, 4};
        auto sub = SubVec{&vec};

        CHECK(sub[5] == 4);
        CHECK(sub[3] == 6);

        auto left = sub.Sub(0, 3);
        auto right = sub.Sub(3, 6);

        CHECK_FALSE(left.empty());
        CHECK_FALSE(right.empty());

        // std::sort(left.begin(), left.end());
        std::sort(right.begin(), right.end());

        CHECK(left.size() == 3);
        CHECK(right.size() == 3);

        CHECK_FALSE(vec.empty());
        CHECK(sub.size() == 6);
        CHECK(Check(vec, {3, 2, 1, 4, 5, 6}));
    }
}
