#ifndef STA_UTIL_HASHER_H
#define STA_UTIL_HASHER_H

#include <cstdint>

namespace Util {

// String hash function. It is a modified version of MurmurHash3_x86_32.
//
// #### Input
//
// - key: A string.
//
// #### Output
//
// - An 32-bit integer.
//
std::uint32_t hash_str(const std::string& str);

} // namespace Util

#endif // STA_UTIL_HASHER_H
