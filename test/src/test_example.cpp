#include <cassert>
#include <iostream>

#include "test/src/util/myrand.h"

void test1(void) {
    std::cerr << "test1():\n";

    // Will I get 3 consecutive same number from myrand()?
    double a = myrand();
    double b = myrand();
    double c = myrand();

    assert(!(a == b && b == c) && 
        "Error: a = b = c, seems like myrand() have some problem");

    std::cerr << "test1() passed.\n";
}

void test2(void) {
    std::cerr << "test2():\n";

    // Swap without using temporary variable.
    int a = 13, b = 30;
    a = a ^ b;
    b = a ^ b;
    a = a ^ b;

    assert(a == 30 && "Error: a != 30, swap failed.");
    assert(b == 13 && "Error: b != 13, swap failed.");

    std::cerr << "test2() passed.\n";
}
