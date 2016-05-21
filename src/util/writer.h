#ifndef STA_UTIL_WRITER_H
#define STA_UTIL_WRITER_H

#include <string>
#include <vector>

#include "cir/circuit.h"

namespace Util {

class Writer {
public:

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
    // - 0: Execute successfully.
    // - 1: Cannot open file.
    // - 2: Other.
    //
    int write(const std::vector<Cir::Path>&     paths,
              const std::vector<Cir::InputVec>& input_vecs,
              const std::string&                outfile = "");
};

}

#endif // STA_UTIL_WRITER_H
