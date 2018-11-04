#ifndef EUPHORIA_FNV1A_H
#define EUPHORIA_FNV1A_H

#include <cstdint>

constexpr std::uint64_t
fnv1a(const char* const str, std::uint64_t hash = 0xcbf29ce484222325)
{
  // reference:
  // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
  return *str == 0 ? hash : fnv1a(str + 1, (hash ^ *str) * 0x100000001b3);
}

#endif  // EUPHORIA_FNV1A_H