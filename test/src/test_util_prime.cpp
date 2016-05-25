#include <iostream>

#include "test/src/util/util.h"
#include "util/prime.h"

void test_prime_gt(void) {
    std::cerr << __FUNCTION__ << "():\n";

    size_t n;

    ASSERT((n = Util::prime_gt(1)) == 2,
        << "n = " << n << " != 2");

    ASSERT((n = Util::prime_gt(100)) == 197,
        << "n = " << n << " != 197");

    ASSERT((n = Util::prime_gt(26972159856)) == 26972159857,
        << "n = " << n << " != 26972159857");

    ASSERT((n = Util::prime_gt(26972159857)) == 26972159857,
        << "n = " << n << " != 26972159857");

    ASSERT((n = Util::prime_gt(51437)) == 51437,
        << "n = " << n << " != 51437");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
