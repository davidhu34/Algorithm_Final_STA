#include "sta/src/ana/analyzer.h"

#include <iostream>
#include <queue>
#include <stack>

#include "minisat_blbd/src/core/Solver.h"

// Assign arrival time.
//
// #### Input
//
// - `circuit`
//
static void assign_arrival_time(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;

    // Assign 0 to all gate's tag. It means the number of fan-in of
    // that gate which has arrived.
    //
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->tag = 0;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->tag = 0;
    }

    // For all gate inside queue, all of its fan-in has arrived.
    // Every gate inside queue has already known arrival time.
    // This queue should be sorted according to arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* pi = cir.primary_inputs[i];
        pi->arrival_time = 0;
        q.push(pi);
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        // Tell gate's fan-outs that it has arrived.
        for (size_t i = 0; i < gate->tos.size(); ++i) {
            Gate* fanout = gate->tos[i];
            fanout->tag += 1;
            
            // If that fan-out's fan-ins have all arrived
            if (fanout->tag == fanout->froms.size()) {
                
                // Assign arrival time to that fan-out.
                fanout->arrival_time = gate->arrival_time + 1;

                // Add that fan-out to queue.
                q.push(gate->tos[i]);
            }
        }
    }
}

// Add NAND clause into solver.
//
// #### Input
//
// - `A`
// - `B`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NAND_clause(Minisat::Var A, 
                            Minisat::Var B, 
                            Minisat::Var C, 
                            Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(B), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(A, 1), mkLit(B, 1), mkLit(C, 1))) {
        return false;
    }

    return true;
}

// Add NOR clause into solver.
//
// #### Input
//
// - `A`
// - `B`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NOR_clause(Minisat::Var A, 
                           Minisat::Var B, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A, 1), mkLit(C, 1))) {
        return false;
    }
    if (!solver.addClause(mkLit(B, 1), mkLit(C, 1))) {
        return false;
    }
    if (!solver.addClause(mkLit(A), mkLit(B), mkLit(C))) {
        return false;
    }

    return true;
}

// Add NOR clause into solver.
//
// #### Input
//
// - `A`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NOT_clause(Minisat::Var A, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(A, 1), mkLit(C, 1))) {
        return false;
    }

    return true;
}

int Sta::Ana::find_sensitizable_paths(
    Cir::Circuit&               cir,
    std::vector<Cir::Path>&     paths,
    std::vector<Cir::InputVec>& input_vecs) {

    // Assign arrival time.
    assign_arrival_time(cir);

    Minisat::Solver solver;

    // Assign SAT variable to gate.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        cir.primary_inputs[i]->var = solver.newVar();
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->var = solver.newVar();
    }

    // Add clauses into solver.
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        Cir::Gate* gate = cir.logic_gates[i];
        bool       success;

        switch (gate->module) {
        case Cir::Module::NAND:
            success = add_NAND_clause(gate->froms[0]->var,
                                      gate->froms[1]->var,
                                      gate->var,
                                      solver);
            break;

        case Cir::Module::NOR:
            success = add_NOR_clause(gate->froms[0]->var,
                                     gate->froms[1]->var,
                                     gate->var,
                                     solver);
            break;

        case Cir::Module::NOT:
            success = add_NOT_clause(gate->froms[0]->var,
                                     gate->var,
                                     solver);
            break;
        } // switch (gate->module)

        if (!success) {
            std::cerr << "Error: Clause conflicts while adding clause.\n";
            return 1;
        }
    }

    // Simplify

    return 0;
}
