#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#include <string>
#include <vector>
#include <sstream>

namespace assertlib
{
  struct AssertValueArg
  {
    std::string value;
    template <typename T>
    AssertValueArg(const T& t)
    {
      std::ostringstream ss;
      ss << t;
      value = ss.str();
    }
  };
  void
  StartThrowing();
  void
  OnAssert(
      const char* const                  expression,
      int                                line,
      const char* const                  file,
      const char* const                  argstr,
      const std::vector<AssertValueArg>& arguments,
      const char* const                  function);
}

// todo: stb libraries and rapidjson aren't using our assert
#define ASSERT(x)                                               \
  do                                                            \
  {                                                             \
    if(x)                                                       \
    {                                                           \
    }                                                           \
    else                                                        \
    {                                                           \
      ::assertlib::OnAssert(                                    \
          #x,                                                   \
          __LINE__,                                             \
          __FILE__,                                             \
          "",                                                   \
          {},                                                   \
          static_cast<const char* const>(__PRETTY_FUNCTION__)); \
    }                                                           \
  } while(false)

#define ASSERTX(x, ...)                                         \
  do                                                            \
  {                                                             \
    if(x)                                                       \
    {                                                           \
    }                                                           \
    else                                                        \
    {                                                           \
      ::assertlib::OnAssert(                                    \
          #x,                                                   \
          __LINE__,                                             \
          __FILE__,                                             \
          #__VA_ARGS__,                                         \
          {__VA_ARGS__},                                        \
          static_cast<const char* const>(__PRETTY_FUNCTION__)); \
    }                                                           \
  } while(false)

#define DIE(message)     \
  ::assertlib::OnAssert( \
      message,           \
      __LINE__,          \
      __FILE__,          \
      "",                \
      {},                \
      static_cast<const char* const>(__PRETTY_FUNCTION__))

#endif  // CORE_ASSERT_H
