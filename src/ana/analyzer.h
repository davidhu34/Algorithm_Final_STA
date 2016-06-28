#ifndef STA_ANA_ANALYZER_H
#define STA_ANA_ANALYZER_H

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/subcircuit.h"

namespace Sta {
namespace Ana {

// Find true paths.
//
// Input
//
// - `circuit`
// - `time_constraint`
// - `slack_constraint`
//
// Please make sure the connection of gates inside the circuit is
// correct.
//
// Gates inside the circuit will be modified, but the circuit won't be
// modified.
//
// Output
//
// - `paths`
// - `values`
// - `input_vecs`
//
// `paths` is vector of Path. Path is vector of Gate. They are found
// true paths. Note that my path is reversed. It starts from PO and
// ends at PI. 
//
// `values` is vector of PathValue. PathValue is vector of bool.
// path_value[i] record value of gate at path[i]. values[i] record
// path_value of path at paths[i].
//
// `input_vecs` is vector of InputVec. InputVec is vector of bool.
// input_vec[i] record value of PI at primary_inputs[i]. InputVec is
// an assignment of primary inputs that can make a path to become a
// true path. input_vecs[i] record InputVec of path at paths[i].
//
// New data will be added into them directly without clearing them
// first. Please make sure that they are empty.
//
// The function will return true if operation succeeded. Actually it
// will always return true. If any error happened, it will just abort
// the program.
//
bool find_true_paths(
    const Cir::Circuit&          circuit,
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
// Please make sure that the path connection is correct, i.e. two
// consecutive gates in a path is really connected.
//
// It will modify gates inside the circuit.
//
// Return false if any error is found. Return true otherwise. It will
// print out any error it found.
//
bool verify_true_path_set(
    const Cir::Circuit&                circuit,
    int                                time_constraint,
    int                                slack_constraint,
    const std::vector<Cir::Path>&      paths,
    const std::vector<Cir::PathValue>& values,
    const std::vector<Cir::InputVec>&  input_vecs);

// Verify whether a path is a true path.
//
// Please make sure that the path connection is correct.
//
// Return true if the path is a true path. Return false otherwise.
//
// It will fill up g1, g2 and gY with gates that make this path
// cannot evaluate to true path.
//
bool verify_true_path(const Cir::Circuit&  cir,
                      const Cir::Path&     path,
                      const Cir::InputVec& input_vec,
                      const Cir::Gate*&    g1,
                      const Cir::Gate*&    g2,
                      const Cir::Gate*&    gY        );

bool verify_true_path(const Cir::Circuit&  cir,
                      const Cir::Path&     path,
                      const Cir::InputVec& input_vec);

// Calculate value and arrival time of all gates. 
//
// It will modify gates inside the circuit.
//
void calculate_value_and_arrival_time(
    const Cir::Circuit&  circuit,
    const Cir::InputVec& input_vec);

// Calculate maximum arrival time.
//
// It will modify gates inside the circuit.
//
void calculate_max_arrival_time(const Cir::Circuit& circuit);

// Calculate minimum arrival time.
//
// It will modify gates inside the circuit.
//
void calculate_min_arrival_time(const Cir::Circuit& circuit);

// Check whether there is any conflict in the circuit with this
// assignment of value and arrival time.
//
// `path` is vector of gate that you are trying to make it to become
// a true path. `subpath` is additional gates that you've touched
// while trying to make a path to become a true path.
//
// It will modify gates inside the circuit. It will set PIs to values
// that can prove that this is a true path, if no conflict occur.
//
// Please make sure that all gates' min and max arrival time is
// calculated.
//
// Return true if no conflict. Return false otherwise.
//
bool no_conflict_2(const Cir::Subcircuit& cir,
                   const Cir::PathCopy&   path,
                   Cir::PathCopy          subpath,
                   Cir::InputVec&         input_vec);

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_ANALYZER_H
