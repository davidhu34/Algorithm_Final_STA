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

} // namespace Ana
} // namespace Sta

#endif // STA_ANA_HELPER_H
