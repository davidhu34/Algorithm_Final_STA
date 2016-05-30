#ifndef STA_CIR_PARSER_H
#define STA_CIR_PARSER_H

#include <vector>

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
// - Error code.
//     - 0: Execute successfully.
//     - 1: Cannot open file.
//     - 2: Other.
//
int parse(const std::vector<const char*>& input_files,
          Circuit&                        circuit     );
          
} // namespace Cir
} // namespace Sta

#endif // STA_CIR_PARSER_H
