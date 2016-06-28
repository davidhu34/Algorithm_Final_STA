#include "sta/src/ana/helper.h"

#include <assert.h>
#include <iostream>

// Add NAND clause into solver.
// Return True if success, false otherwise.
//
static bool add_NAND_clause(Minisat::Var A, 
                            Minisat::Var B, 
                            Minisat::Var C, 
                            Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (solver.addClause(mkLit(A), mkLit(C)) &&
        solver.addClause(mkLit(B), mkLit(C)) &&
        solver.addClause(mkLit(A, 1), mkLit(B, 1), mkLit(C, 1))) {

        return true;
    }

    return false;
}

// Add NOR clause into solver.
// Return True if success, false otherwise.
//
static bool add_NOR_clause(Minisat::Var A, 
                           Minisat::Var B, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (solver.addClause(mkLit(A, 1), mkLit(C, 1)) &&
        solver.addClause(mkLit(B, 1), mkLit(C, 1)) &&
        solver.addClause(mkLit(A), mkLit(B), mkLit(C))) {

        return true;
    }

    return false;
}

// Add NOR clause into solver.
// Return True if success, false otherwise.
//
static bool add_NOT_clause(Minisat::Var A, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (solver.addClause(mkLit(A), mkLit(C)) &&
        solver.addClause(mkLit(A, 1), mkLit(C, 1))) {

        return true;
    }

    return false;
}

bool Sta::Ana::init_solver(const Cir::Circuit& cir, 
                           Minisat::Solver&    solver) {

    using Cir::Gate;
    using Cir::Module;

    // Assign SAT variable to gate.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        cir.primary_inputs[i]->var = solver.newVar();
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->var = solver.newVar();
    }
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->var = 
            cir.primary_outputs[i]->froms[0]->var;
    }

    // Add clauses into solver.
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        Gate* gate = cir.logic_gates[i];
        bool  success;

        switch (gate->module) {
        case Module::NAND2:
            success = add_NAND_clause(gate->froms[0]->var,
                                      gate->froms[1]->var,
                                      gate->var,
                                      solver);
            break;

        case Module::NOR2:
            success = add_NOR_clause(gate->froms[0]->var,
                                     gate->froms[1]->var,
                                     gate->var,
                                     solver);
            break;

        case Module::NOT1:
            success = add_NOT_clause(gate->froms[0]->var,
                                     gate->var,
                                     solver);
            break;
        } // switch (gate->module)

        if (!success) {
            std::cerr << "Error: Clause conflicts while adding clause.\n";
            return false;
        }
    }

    // Simplify
    if (!solver.simplify()) {
        std::cerr << "Error: Clause conflicts while simplifying.\n";
        return false;
    }
    
    return true;
}
