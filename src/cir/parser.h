#ifndef STA_CIR_PARSER_H
#define STA_CIR_PARSER_H

#include <vector>
#include <string>

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Cir {

// Parse verilog files into circuit. This parser can only read verilog
// file with similar format to those given in Problem D: Static Timing
// Analysis. And it does not do any (syntax or logic) error detection.
//
// #### Input
//
// - `input_files`: Name of files to be parsed.
// - `circuit`    : Makesure circuit is empty before passing it to this
//                  function.
//
// #### Output
//
// - `circuit`: A circuit to be returned.
//
// Return true is parse succeed. Return false otherwise.
//
bool parse(const std::vector<const char*>& input_files,
           Circuit&                        circuit     );

bool parse(const char* in_file_1,
           Circuit&    circuit     );

bool parse(const char* in_file_1,
           const char* in_file_2,
           Circuit&    circuit     );

bool parse(const char* in_file_1,
           const char* in_file_2,
           const char* in_file_3,
           Circuit&    circuit     );

// Extract paths, values and input_vecs from `true_path_set_file`.
//
// #### Input
//
// - `true_path_set_file`
// - `circuit`
// - `time_constraint`
//
// #### Ouput
//
// - `paths`
// - `values`
// - `input_vecs`
//
// Return true is parse succeed. Return false otherwise.
//
bool parse_true_path_set(
    const std::string&           true_path_set_file,
    const Circuit&               circuit,
    int                          time_constraint,
    int                          slack_constraint,
    std::vector<Cir::Path>&      paths,
    std::vector<Cir::PathValue>& values,
    std::vector<Cir::InputVec>&  input_vecs);
          
} // namespace Cir
} // namespace Sta

#endif // STA_CIR_PARSER_H
