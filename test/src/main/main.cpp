#include <iostream>

#include "testcir.h"
#include "testutil.h"

int main(int argc, const char* argv) {
    std::cout << "Test module cir:\n";
    testcir(argc, argv);

    std::cout << "Test module util:\n";
    testutil(argc, argv);

    return 0;
}
