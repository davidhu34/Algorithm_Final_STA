#ifndef STA_ANA_HELPER_H
#define STA_ANA_HELPER_H

#include "sta/src/cir/circuit.h"
#include "minisat/src/core/Solver.h"

namespace Sta {
namespace Ana {

// Initialize SAT Solver with circuit. It will set gate's var and add
// clauses into solver.
//
// Please make sure circuit is connected correctly.
//
// Return true if success. Return false otherwise.
//
bool init_solver(const Cir::Circuit& cir, Minisat::Solver& solver);

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
bool no_conflict(const Cir::Circuit&   cir,
                 const Cir::Path&      path,
                 const Cir::PathValue& path_value,
                 const Cir::Path&      subpath,
                 const Cir::PathValue& subpath_value,
                 Minisat::Solver&      solver       );

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_HELPER_H
