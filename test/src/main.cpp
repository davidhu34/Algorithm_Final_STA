#include <iostream>

// Put test function prototype here.
// Please implement them in other file.
void test1(void);
void test2(void);
int test_circuit(int argc, const char* argv[]);
void test_get_token(void);

int main(int argc, const char* argv[]) {
    // Call test function here.
    // Comment out those you don't want to test.
    test1();
    test2();
    //testcircuit(arc, argv);
    test_get_token();

    return 0;
}
