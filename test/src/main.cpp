#include <iostream>

// Put test function prototype here.
// Please implement them in other file.

// test_example.cpp
void test1(void);
void test2(void);

// test_cir_circuit.cpp
int test_circuit(int argc, const char* argv[]);

int main(int argc, const char* argv[]) {
    // Call test function here.
    // Comment out those you don't want to test.
    
    std::cerr << "test_example.cpp\n";
    test1();
    test2();

    //std::cerr << "\ntest_cir_circuit.cpp";
    test_circuit(argc, argv);

    return 0;
}
