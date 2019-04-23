#ifndef CORE_ENUM_TO_STRING_H
#define CORE_ENUM_TO_STRING_H

#include <string>
#include <map>
#include <vector>
#include <queue>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/levenshtein.h"

template<typename T>
struct MatchedEnum
{
  bool single_match = false;
  std::vector<T> values;
};

template<typename T>
struct EnumToStringImpl
{
  std::map<T, std::string> enum_to_string;
  std::map<std::string, T> string_to_enum;

  // todo: replace with initializer list?
  EnumToStringImpl<T>& Add(const std::string& name, T t)
  {
    enum_to_string.insert(std::make_pair(t, name));
    string_to_enum.insert(std::make_pair(ToLower(name), t));
    return *this;
  }

  std::string ToString(T t) const
  {
    auto found = enum_to_string.find(t);
    if(found != enum_to_string.end())
    {
      return found->second;
    }
    DIE("Enum not added");
    return "???";
  }

  std::vector<std::string> ListNames() const
  {
    std::vector<std::string> ret;

    for(auto entry: enum_to_string)
    {
      ret.emplace_back(entry.second);
    }

    return ret;
  }

  MatchedEnum<T> Match(const std::string& input, size_t max_size) const
  {
    auto found = string_to_enum.find(ToLower(input));
    if(found != string_to_enum.end())
    {
      return MatchedEnum<T>{true, {found->second}};
    }
    struct Match { T t; unsigned long changes;
      bool operator<(const Match& rhs) const {return changes<rhs.changes;} };
    std::priority_queue<Match> matches;
    for(auto entry: enum_to_string)
    {
      const auto t = entry.first;
      const auto str = entry.second;
      const auto changes = LevenshteinDistance(str, input);
      matches.push({t, changes});
      if(matches.size() > max_size)
      {
        matches.pop();
      }
    }
    auto ret = MatchedEnum<T>{};
    while(!matches.empty())
    {
      ret.values.push_back(matches.top().t);
      matches.pop();
    }
    return ret;
  }
};

template<typename T>
struct GetEnumToString
{
  enum { IsDefined = 0 };

  // no implmenentation: only here for template specialization
  static const EnumToStringImpl<T>& EnumValues();
};


template<typename T>
MatchedEnum<T> StringToEnum(const std::string& input, size_t max_size = 5)
{
  return GetEnumToString<T>::EnumValues().Match(input, max_size);
}

template<typename T>
std::string EnumToString(T t)
{
  return GetEnumToString<T>::EnumValues().ToString(t);
}

template<typename T>
std::vector<std::string> EnumToString(const std::vector<T>& ts)
{
  std::vector<std::string> ret;
  for(auto t: ts)
  {
    ret.emplace_back(GetEnumToString<T>::EnumValues().ToString(t));
  }
  return ret;
}

template<typename T>
std::vector<std::string> EnumToString()
{
  return GetEnumToString<T>::EnumValues().ListNames();
}


#define BEGIN_ENUM_LIST(T) template<> struct GetEnumToString<T> { enum { IsDefined = 1 }; static const ::EnumToStringImpl<T>& EnumValues() { static const auto r = ::EnumToStringImpl<T>{}
#define ENUM_VALUE(T, V) .Add(#V, T::V)
#define END_ENUM_LIST() ; return r; } };

#endif  // CORE_ENUM_TO_STRING_H

