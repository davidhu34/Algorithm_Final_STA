#ifndef STA_UTIL_HASHER_H
#define STA_UTIL_HASHER_H

#include <stdint.h>
#include <string>

namespace Sta {
namespace Util {

// String hash function. It is a modified version of MurmurHash3_x86_32.
uint32_t hash_str(const std::string& str);

// Pointer hash function.
inline uint32_t hash_ptr(const void* ptr) {
    return reinterpret_cast<uint64_t>(ptr) >> 2;
}

typedef uint32_t (*StrHashFunc)(const std::string& str);
typedef uint32_t (*PtrHashFunc)(const void* ptr);

} // namespace Util
} // namespace Sta

#endif // STA_UTIL_HASHER_H
