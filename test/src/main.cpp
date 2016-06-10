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

// test_util_hash_set.cpp
void test_hash_set(void);

// test_util_hash_map.cpp
void test_hash_map(void);

// test_minisat_blbd.cpp
void test_minisat(void);
void test_minisat_2(void);

// test_cir_parser_inc.cpp
void test_get_token(void);
void test_parse_module_NAND2(void);
void test_parse_module_NOR2(void);
void test_parse_module_NOT1(void);

// test_cir_parser.cpp
void test_parse(void);

// test_cir_copy.cpp
void test_circuit_copy(void);

// test_ana_analyzer.cpp
void test_find_true_paths(void);

// test_cir_writer.cpp
void test_write(void);

// test_ana_verify.cpp
void test_verify_true_path_set(void);

// test_overall.cpp
void test_overall(void);

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

    std::cerr << "\ntest_util_hash_set.cpp\n";
    test_hash_set();

    std::cerr << "\ntest_util_hash_map.cpp\n";
    test_hash_map();

    std::cerr << "\ntest_minisat_blbd.cpp\n";
    test_minisat();
    test_minisat_2();

    std::cerr << "\ntest_cir_parser_inc.cpp\n";
    test_get_token();
    test_parse_module_NAND2();
    test_parse_module_NOR2();
    test_parse_module_NOT1();

    std::cerr << "\ntest_cir_parser.cpp\n";
    test_parse();

    std::cerr << "\ntest_cir_copy.cpp\n";
    test_circuit_copy();

    std::cerr << "\ntest_ana_analyzer.cpp\n";
    test_find_true_paths();

    std::cerr << "\ntest_cir_writer.cpp\n";
    test_write();

    std::cerr << "\ntest_ana_verify.cpp\n";
    test_verify_true_path_set();

    std::cerr << "\ntest_overall.cpp\n";
    test_overall();
    
    return 0;
}
