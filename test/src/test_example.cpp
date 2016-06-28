#include <iostream>

#include "sta/test/src/util/util.h"
#include "sta/test/src/util/myrand.h"

void test1(void) {
    using TestUtil::myrand;

    // Will I get 3 consecutive same number from myrand()?
    double a = myrand();
    double b = myrand();
    double c = myrand();

    ASSERT(!(a == b && b == c),
        << "We got three consecutive same number:\n"
        << "a = " << a << "\n"
        << "b = " << b << "\n"
        << "c = " << c << "\n");
}

void test2(void) {
    // Swap without using temporary variable.
    int a = 13, b = 30;
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;

    ASSERT(a == 30 && b == 13,
        << "a = " << a << "\nb = " << b << "\nSwap failed.\n");
}
