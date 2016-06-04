#ifndef STA_CIR_WRITER_H
#define STA_CIR_WRITER_H

#include <string>
#include <vector>

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Cir {

// Write paths and input vectors in format specified by requirement.md
// into stdout or a file.
//
// #### Input
//
// - paths     : To be printed. Path is defined in cir module.
// - input_vecs: To be printed. InputVec is defined in cir module.
// - outfile   : Output file name, default to empty. If it is empty,
//               write to stdout.
//
// #### Output
//
// - Error code.
//     - 0: Execute successfully.
//     - 1: Cannot open file.
//     - 2: Other.
//
int write(const Circuit&                          circuit,
          int                                     time_constraint,
          int                                     slack_constraint,
          const std::vector<Path>&                paths,
          const std::vector< std::vector<bool> >& values,
          const std::vector<InputVec>&            input_vecs,
          const std::string&                      outfile = "");

// Dump circuit connection information. This is for circuit checking.
// If `dump_file` is empty string, dump to stdout. Return 0 if
// operation succes. Return 1 otherwise.
// 
int dump(const Circuit&     cir,
         const std::string& dump_file = "");

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_WRITER_H
