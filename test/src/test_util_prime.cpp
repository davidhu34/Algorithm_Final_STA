#include <iostream>

#include "sta/test/src/util/util.h"
#include "sta/src/util/prime.h"

void test_prime_gt(void) {
    using Sta::Util::prime_gt;

    size_t n;

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
}
