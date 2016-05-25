#ifndef STA_UTIL_PRIME_H
#define STA_UTIL_PRIME_H

#include <cstddef>

namespace Util {

// Return a prime number greater than n. This prime number is not
// necessary the smallest prime number that is greater than n.
// The main usage of this function is provide it as bucket size
// for hash map, so that the hash value has minimum collision after
// modulo operation. The maximum prime at here is 26972159857.
// If n is greater than that it will crash.
//
// #### Input
//
// - n
//
// #### Output
//
// - A prime number greater than n.
//
size_t prime_gt(size_t n);

} // namespace Util

#endif // STA_UTIL_PRIME_H
