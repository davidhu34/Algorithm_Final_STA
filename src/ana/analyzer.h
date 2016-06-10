#ifndef STA_ANA_ANALYZER_H
#define STA_ANA_ANALYZER_H

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Ana {

// Find true paths.
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
// - `paths`     : Found true paths.
// - `values`    : Record value of gates of paths.
// - `input_vecs`: Corresponding input vector that cause a path become
//                 true path. Path start from PO, end at PI.
// - Return an error code: 0 means successful; 1 means error encountered.
//
int find_true_paths(
    Cir::Circuit&                circuit,
    int                          time_constraint,
    int                          slack_constraint,
    std::vector<Cir::Path>&      paths,
    std::vector<Cir::PathValue>& values,
    std::vector<Cir::InputVec>&  input_vecs);

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
int verify_true_path_set(
    Cir::Circuit&                      circuit,
    int                                time_constraint,
    int                                slack_constraint,
    const std::vector<Cir::Path>&      paths,
    const std::vector<Cir::PathValue>& values,
    const std::vector<Cir::InputVec>&  input_vecs);

// Verify a whether a path is a true path. Return 0 if it is true.
// It will fill up g1, g2 and gY with gates that make this path
// cannot evaluate to true path.
//
int verify_true_path(Cir::Circuit&        cir,
                     const Cir::Path&     path,
                     const Cir::InputVec& input_vec,
                     const Cir::Gate*&    g1,
                     const Cir::Gate*&    g2,
                     const Cir::Gate*&    gY        );

int verify_true_path(Cir::Circuit&        cir,
                     const Cir::Path&     path,
                     const Cir::InputVec& input_vec);

// Calculate value and arrival time of all gates. Please initialize 
// PI to required value and all other gate to undefined value before
// passing circuit to me.
//
void calculate_value_and_arrival_time(Cir::Circuit& circuit);

// Calculate maximum arrival time.
void calculate_max_arrival_time(Cir::Circuit& circuit);

// Calculate minimum arrival time.
void calculate_min_arrival_time(Cir::Circuit& circuit);

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_ANALYZER_H
