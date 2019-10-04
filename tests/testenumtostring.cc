#include "core/enumtostring.h"

#include "tests/utils.h"

namespace euco = euphoria::core;

namespace
{
  enum class MyEnum
  {
    X, Cat, ReallyLongValue
  };
}

DECLARE_ENUM_LIST(MyEnum)
BEGIN_ENUM_LIST(MyEnum)
  ENUM_VALUE(MyEnum, X)
  ENUM_VALUE(MyEnum, Cat)
  ENUM_VALUE(MyEnum, ReallyLongValue)
END_ENUM_LIST()

TEST_CASE("enum2string", "[enum2string]")
{
  SECTION("single arg ToString")
  {
    CHECK(euco::EnumToString(MyEnum::X) == "X");
    CHECK(euco::EnumToString(MyEnum::Cat) == "Cat");
    CHECK(euco::EnumToString(MyEnum::ReallyLongValue) == "ReallyLongValue");
  }

  SECTION("operator<<")
  {
    const auto e = MyEnum::Cat;
    
    std::stringstream ss;
    ss << e;
    
    CHECK(ss.str() == "Cat");
  }

  SECTION("no arg ToString")
  {
    CHECK_THAT(euco::EnumToString<MyEnum>(), CATCH_IS_VECTOR(std::string, "X", "Cat", "ReallyLongValue"));
  }

  SECTION("vector arg ToString")
  {
    const auto vec = std::vector<MyEnum>{ MyEnum::Cat, MyEnum::X, MyEnum::Cat };
    CHECK_THAT(euco::EnumToString<MyEnum>(vec), CATCH_IS_VECTOR(std::string, "Cat", "X", "Cat"));
  }

  SECTION("match different cases")
  {
    auto name = GENERATE_AS(std::string, "cat", "CAT", "Cat", "cAt");
    const auto match = euco::StringToEnum<MyEnum>(name);
    CHECK(match.single_match);
    CHECK_THAT(match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::Cat));
  }

  SECTION("match suggestions")
  {
    const auto match = euco::StringToEnum<MyEnum>("y", 2);
    CHECK_FALSE(match.single_match);
    CHECK_THAT(match.values, CATCH_IS_VECTOR(MyEnum, MyEnum::X, MyEnum::Cat));
  }
}

