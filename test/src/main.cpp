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

// test_minisat_blbd.cpp
void test_minisat(void);

// test_cir_circuit.cpp
void test_circuit(void);
void test_circuit_state(void);

// test_ana_find.cpp
void test_find_true_paths(void);

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

    std::cerr << "\ntest_minisat_blbd.cpp\n";
    test_minisat();

    std::cerr << "\ntest_cir_circuit.cpp\n";
    test_circuit();
    test_circuit_state();

    //std::cerr << "\ntest_ana_find.cpp\n";
    //test_find_true_paths();

    return 0;
}
