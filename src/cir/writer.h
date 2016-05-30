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
int write(const std::vector<Sta::Cir::Path>&     paths,
          const std::vector<Sta::Cir::InputVec>& input_vecs,
          const std::string&                     outfile = "");

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_WRITER_H
