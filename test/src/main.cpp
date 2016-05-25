#include <iostream>

// Put test function prototype here.
// Please implement them in other file.

// test_example.cpp
void test1(void);
void test2(void);

// test_util_hasher.cpp
void test_hash_str(void);

// test_util_prime.cpp
void test_prime_gt(void);

// test_util_hash_map.cpp
void test_hash_map(void);

// test_cir_parser_inc.cpp
void test_get_token(void);
void test_parse_module_NAND2(void);
void test_parse_module_NOR2(void);
void test_parse_module_NOT1(void);

// test_cir_circuit.cpp
int test_circuit(int argc, const char* argv[]);

int main(int argc, const char* argv[]) {
    // Call test function here.
    // Comment out those you don't want to test.
    
    std::cerr << "test_example.cpp\n";
    test1();
    test2();
    
    std::cerr << "\ntest_util_hasher.cpp\n";
    test_hash_str();

    std::cerr << "\ntest_util_prime.cpp\n";
    test_prime_gt();

    std::cerr << "\ntest_util_hash_map.cpp\n";
    test_hash_map();

    std::cerr << "\ntest_cir_parser_inc.cpp\n";
    test_get_token();
    test_parse_module_NAND2();
    test_parse_module_NOR2();
    test_parse_module_NOT1();

    //std::cerr << "\ntest_cir_circuit.cpp";
    //test_circuit(argc, argv);

    return 0;
}
