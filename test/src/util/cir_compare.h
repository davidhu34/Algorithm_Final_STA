#ifndef STA_TEST_UTIL_CIR_COMPARE_H
#define STA_TEST_UTIL_CIR_COMPARE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "test/src/util/util.h"

namespace TestUtil {

// Check compare state files of two circuits. State file of a circuit
// has the following format:
//
// ```
// -circuit_name          case0
// -pi_count              13
// -po_count              30
// -gate_count            78
// -nand2_input_pin_name  A B
// -nand2_output_pin_name Y
// -nor2_input_pin_name   A B
// -nor2_output_pin_name  Y
// -not1_input_pin_name   A
// -not1_output_pin_name  Y
// -u1/NAND2              A:u7 B:u8
// -u9/NOR2               A:u2 B:u7
// ...
// ```
// 
// `-circuit_name`, `case0`, `-pi_count`, `13`, ... are tokens.
// A token is consecutive printable characters excluding whitespace.
// You can put any number of whitespace between token. A token start
// with `-` is key token. All other following tokens are value tokens.
//
// cir_compare will read this file. For each key token, cir_compare put
// all following value tokens into a set. cir_compare then map that
// key token into the set. After that, cir_compare read another file.
// When it read a key token, it will find the value token set that 
// is mapped by the key token. Then, for each following value token
// read from file, cir_compare remove it from value token set. If it read
// a value token that it can't find in that value token set, it issues
// error. At the end, it output all value token set that is not empty.
// 
// Major part of the key token are gates. All token following it are
// fanins of it. `-U1/NAND2 A:U13 B:U30` means U1 input from U13 through
// pin A, input from U30 through pin B.
//
// #### Input
//
// - file1: State file of circuit 1.
// - file2: State file of circuit 2.
//
void cir_compare(const std::string& file1,
                 const std::string& file2);

} // namespace TestUtil

#endif // STA_TEST_UTIL_CIR_COMPARE_H
