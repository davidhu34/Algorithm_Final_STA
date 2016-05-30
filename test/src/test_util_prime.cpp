#include <iostream>

#include "sta/test/src/util/util.h"
#include "sta/src/util/prime.h"

void test_prime_gt(void) {
    std::cerr << __FUNCTION__ << "():\n";

    size_t n;
    using Sta::Util::prime_gt;

    ASSERT((n = prime_gt(1)) == 2,
        << "n = " << n << " != 2\n");

    ASSERT((n = prime_gt(100)) == 197,
        << "n = " << n << " != 197\n");

    ASSERT((n = prime_gt(26972159856)) == 26972159857,
        << "n = " << n << " != 26972159857\n");

    ASSERT((n = prime_gt(26972159857)) == 26972159857,
        << "n = " << n << " != 26972159857\n");

    ASSERT((n = prime_gt(51437)) == 51437,
        << "n = " << n << " != 51437\n");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
