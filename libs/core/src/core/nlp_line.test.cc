#include "core/nlp_line.h"

#include "tests/approx_equal.h"
#include "tests/stringeq.h"

#include "catch.hpp"


using namespace euphoria::tests;
using namespace euphoria::core;


TEST_CASE("nlp-line", "[nlp]")
{
    SECTION("simple parse")
    {
        const auto result = parse_line("Hello world");
        REQUIRE(result);
        const auto list = *result;
        REQUIRE(string_is_equal(list, {"Hello", "world"}));
    }

    SECTION("to string")
    {
        REQUIRE(string_is_equal("Hello world", line_to_string({"Hello", "world"})));
    }
}

