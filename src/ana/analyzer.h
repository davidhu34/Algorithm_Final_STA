#ifndef STA_ANA_ANALYZER_H
#define STA_ANA_ANALYZER_H

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Ana {

// Find sensitizable paths.
//
// #### Input
//
// - `circuit`         : It will be modified. Actually is the gate inside
//                       the circuit will be modified.
// - `time_constraint`
// - `slack_constraint`
//
// #### Output
//
// - `paths`     : Found sensitizable paths.
// - `values`    : Record value of gates of paths.
// - `input_vecs`: Corresponding input vector that cause a path become
//                 true path. Path start from PO, end at PI.
// - Return an error code: 0 means successful; 1 means error encountered.
//
int find_sensitizable_paths(
    Cir::Circuit&                     circuit,
    int                               time_constraint,
    int                               slack_constraint,
    std::vector<Cir::Path>&           paths,
    std::vector< std::vector<bool> >& values,
    std::vector<Cir::InputVec>&       input_vecs);

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_ANALYZER_H
