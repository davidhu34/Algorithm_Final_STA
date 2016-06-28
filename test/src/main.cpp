#include <iostream>

#define CALL(function_name)                        \
    void function_name(void);                      \
    std::cerr << #function_name << "():\n";        \
    function_name();                               \
    std::cerr << #function_name << "() passed.\n";

int main(void) {
    // Call test function here.
    // Comment out those you don't want to test.
    
    /*std::cerr << "test_example.cpp\n";
    CALL(test1)
    CALL(test2)

    std::cerr << "\ntest_util_hasher.cpp\n";
    CALL(test_hash_str)
    CALL(test_hash_ptr)

    std::cerr << "\ntest_util_prime.cpp\n";
    CALL(test_prime_gt)

    std::cerr << "\ntest_util_hash_set.cpp\n";
    CALL(test_hash_set)

    std::cerr << "\ntest_util_hash_map.cpp\n";
    CALL(test_hash_map)

    std::cerr << "\ntest_minisat.cpp\n";
    CALL(test_minisat)
    CALL(test_minisat_2)

    std::cerr << "\ntest_cir_parse_inc.cpp\n";
    CALL(test_get_token)
    CALL(test_parse_module_NAND2)
    CALL(test_parse_module_NOR2)
    CALL(test_parse_module_NOT1)

    std::cerr << "\ntest_cir_parse.cpp\n";
    CALL(test_parse)

    std::cerr << "\ntest_cir_circuit.cpp\n";
    CALL(test_circuit)

    std::cerr << "\ntest_cir_subcircuit.cpp\n";
    CALL(test_subcircuit)
*/
    std::cerr << "\ntest_ana_find_true_paths.cpp\n";
    CALL(test_find_true_paths)

    //std::cerr << "\ntest_cir_write.cpp\n";
    //CALL(test_write)

    //std::cerr << "\ntest_ana_verify_true_path_set.cpp\n";
    //CALL(test_verify_true_path_set)

    //std::cerr << "\ntest_overall.cpp\n";
    //CALL(test_overall)
    
    return 0;
}
