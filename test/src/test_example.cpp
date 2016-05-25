#include <cassert>
#include <iostream>

#include "test/src/util/util.h"
#include "test/src/util/myrand.h"

void test1(void) {
    std::cerr << __FUNCTION__ << "():\n";

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

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test2(void) {
    std::cerr << __FUNCTION__ << "():\n";

    // Swap without using temporary variable.
    int a = 13, b = 30;
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;

    ASSERT(a == 30 && b == 13,
        << "a = " << a << "\nb = " << b << "\nSwap failed.\n");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
