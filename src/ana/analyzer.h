#ifndef STA_ANA_ANALYZER_H
#define STA_ANA_ANALYZER_H

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Ana {

// Assign arrival time.
//
// #### Input
//
// - `circuit`
//
void assign_arrival_time(Cir::Circuit& circuit);

// Find sensitizable paths.
//
// #### Input
//
// - `circuit`: TODO: Should it modify circuit?
//
// #### Output
//
// - `paths`     : Found sensitizable paths.
// - `input_vecs`: Corresponding input vector that cause a path become
//                 true path.
// - Return an error code: 0 means successful; 1 means error encountered.
//
int find_sensitizable_paths(Cir::Circuit&               circuit,
                            std::vector<Cir::Path>&     paths,
                            std::vector<Cir::InputVec>& input_vecs);

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_ANALYZER_H
