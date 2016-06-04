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

// Verify a true path set. For each path in true path set, check whether
// 
// 1. It is within time constraint and slack constraint, 
// 2. The given input vector can prove that path is a true path. 
// 3. The correctness of value along the path.
// 
// #### Input
//
// - `circuit`
// - `time_constraint`
// - `slack_constraint`
// - `paths`
// - `values`
// - `input_vecs`
//
// #### Output
//
// It will print out any error it found. If there is any error, return 1.
// Else return 0.
//
inline int verify_true_path_set(
    Cir::Circuit&                           circuit,
    int                                     time_constraint,
    int                                     slack_constraint,
    const std::vector<Cir::Path>&           paths,
    const std::vector< std::vector<bool> >& values,
    const std::vector<Cir::InputVec>&       input_vecs) {

    // TODO
    return 0;
}

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_ANALYZER_H
