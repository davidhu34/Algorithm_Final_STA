#ifndef STA_UTIL_HASHER_H
#define STA_UTIL_HASHER_H

#include <string>

// Since C++03 does not offer <cstdint>, I'll have to to define them
// myself.
typedef __UINT8_TYPE__  uint8_t;
typedef __UINT32_TYPE__ uint32_t;

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
uint32_t hash_str(const std::string& str);

} // namespace Util

#endif // STA_UTIL_HASHER_H
