#include "sta/src/ana/analyzer.h"

#include <cassert>
#include <iostream>
#include <queue>
#include <stack>

#include "minisat_blbd/src/core/Solver.h"

// Calculate arrival time.
//
// #### Input
//
// - `circuit`
//
static void calculate_arrival_time(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;
    using Sta::Cir::Module;

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
                if (fanout->module == Module::PO) {
                    fanout->arrival_time = gate->arrival_time;

                    // No need to put PO into queue, because PO does
                    // not have fan-out.
                }
                else {
                    fanout->arrival_time = gate->arrival_time + 1;

                    // Add that fan-out to queue.
                    q.push(fanout);
                }
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

// Call function and create a return point. `n` is the return point
// index. It must be a literal number without any operation. Each
// return point index must be unique. Please do not put semicolon
// after calling this function.
//
#define CALL_FUNCTION(n)     \
    continue_point.push(n);  \
    goto start_function;     \
    case n:

// Perform code substitution. Please do not put semicolon after
// calling these functions.
//
// CAUTION: SURROUND THEM WITH BRACES WHEN USE WITH BRANCH OR LOOP.
//
#define ASSIGN_TRUE(gate)                 \
    gate->value = 1;                      \
    assumptions.push(mkLit(gate->var));   
    
#define ASSIGN_FALSE(gate)                \
    gate->value = 0;                      \
    assumptions.push(mkLit(gate->var, 1));

#define UNDO_ASSIGN(gate)                 \
    assumptions.pop();                    \
    gate->value = 2;

#define PUSH_GATE(gate)                   \
    path.push_back(gate);                 \
    slack -= 1;

#define POP_GATE_1()                      \
    path.pop_back();                      \
    gate = path.back();                   \
    slack += 1;

#define POP_GATE_2()                      \
    path.pop_back();                      \
    gate = path.back();                   \
    gA = gate->froms[0];                  \
    gB = gate->froms[1];                  \
    slack += 1;

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
//
// #### Input
//
// - `po`
// - `cir`
// - `solver`
//
// #### Output
//
// Add sensitizable path to `paths` and an input vector that can make
// that path sensitized.
//
static void trace(Sta::Cir::Gate*                   po,
                  Sta::Cir::Circuit&                cir,
                  int                               time_constraint,
                  int                               slack_constraint,
                  Minisat::Solver&                  solver,
                  std::vector<Sta::Cir::Path>&      paths, 
                  std::vector< std::vector<bool> >& values,
                  std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::Module;
    using Minisat::mkLit;
    using Minisat::toInt;

    // Try to mimic recursive function call without using function.
    // Use continue_point and switch statement to make sure function
    // return to correct point.
    std::stack<int> continue_point;

    // Static variable of that fake function.
    Path                       path(1, po);
    Minisat::vec<Minisat::Lit> assumptions;

    // Temporary/helper variables.
    Gate* gate;
    Gate* gA;
    Gate* gB;
    int   point;
    int   slack = time_constraint;

    // Call function
    continue_point.push(0);
    goto start_function;

pop_function:
    point = continue_point.top(); 
    continue_point.pop();

    switch (point) {

start_function:
    gate = path.back();

    if (slack == 0 && gate->module != Module::PI) {
        goto pop_function;
    }

    if (gate->value == 2) { // Floating
        assert(gate->module == Module::PO);

        gate->value = 0;
        ASSIGN_FALSE(gate->froms[0])
        PUSH_GATE(gate->froms[0])
        CALL_FUNCTION(1)
        POP_GATE_1()
        UNDO_ASSIGN(gate->froms[0])
        gate->value = 2;

        gate->value = 1;
        ASSIGN_TRUE(gate->froms[0])
        PUSH_GATE(gate->froms[0])
        CALL_FUNCTION(2)
        POP_GATE_1()
        UNDO_ASSIGN(gate->froms[0])
        gate->value = 2;
    }

    else if (gate->module == Module::NAND2) {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gA become a true path.
        
        if (gA->arrival_time < gB->arrival_time) {
            if (gA->value == 2) {
                if (gate->value == 1) {
                    ASSIGN_FALSE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(3)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                }
            }
        }

        else if (gA->arrival_time > gB->arrival_time) {
            if (gB->value == 2) {
                ASSIGN_TRUE(gB)

                if (gate->value == 1) {
                    ASSIGN_FALSE(gA)
                }
                else { // gate->value == 0
                    ASSIGN_TRUE(gA)
                }

                PUSH_GATE(gA)
                CALL_FUNCTION(4)
                POP_GATE_2()
                UNDO_ASSIGN(gA)
                UNDO_ASSIGN(gB)
            }
        }

        else { // Both of them have same arrival time.
            if (gate->value == 1) {
                if (gA->value == 2) {
                    ASSIGN_FALSE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(5)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                }
            }

            else { // gate->value == 0
                if (gB->value == 2) {
                    ASSIGN_TRUE(gB)
                    ASSIGN_TRUE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(6)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                    UNDO_ASSIGN(gB)
                }
            }
        }

        // Try to make gB become a true path.

        // Same logic as above, just swap "gA" and "gB"

        if (gB->arrival_time < gA->arrival_time) {
            if (gB->value == 2) {
                if (gate->value == 1) {
                    ASSIGN_FALSE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(7)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                }
            }
        }

        else if (gB->arrival_time > gA->arrival_time) {
            if (gA->value == 2) {
                ASSIGN_TRUE(gA)

                if (gate->value == 1) {
                    ASSIGN_FALSE(gB)
                }
                else { // gate->value == 0
                    ASSIGN_TRUE(gB)
                }

                PUSH_GATE(gB)
                CALL_FUNCTION(8)
                POP_GATE_2()
                UNDO_ASSIGN(gB)
                UNDO_ASSIGN(gA)
            }
        }

        else { // Both of them have same arrival time.
            if (gate->value == 1) {
                if (gB->value == 2) {
                    ASSIGN_FALSE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(9)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                }
            }

            else { // gate->value == 0
                if (gA->value == 2) {
                    ASSIGN_TRUE(gA)
                    ASSIGN_TRUE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(10)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                    UNDO_ASSIGN(gA)
                }
            }
        }
    } // else if (gate->module == Module::NAND2)

    else if (gate->module == Module::NOR2) {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gA become a true path.
        
        if (gA->arrival_time < gB->arrival_time) {
            if (gA->value == 2) {
                if (gate->value == 0) {
                    ASSIGN_TRUE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(11)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                }
            }
        }

        else if (gA->arrival_time > gB->arrival_time) {
            if (gB->value == 2) {
                ASSIGN_FALSE(gB)

                if (gate->value == 1) {
                    ASSIGN_FALSE(gA)
                }
                else { // gate->value == 0
                    ASSIGN_TRUE(gA)
                }

                PUSH_GATE(gA)
                CALL_FUNCTION(12)
                POP_GATE_2()
                UNDO_ASSIGN(gA)
                UNDO_ASSIGN(gB)
            }
        }

        else { // Both of them have same arrival time.
            if (gate->value == 1) {
                if (gA->value == 2) {
                    ASSIGN_FALSE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(13)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                }
            }

            else { // gate->value == 0
                if (gB->value == 2) {
                    ASSIGN_FALSE(gB)
                    ASSIGN_TRUE(gA)
                    PUSH_GATE(gA)
                    CALL_FUNCTION(14)
                    POP_GATE_2()
                    UNDO_ASSIGN(gA)
                    UNDO_ASSIGN(gB)
                }
            }
        }

        // Try to make gB become a true path

        // Same logic as above, just swap "gA" and "gB"
        
        if (gB->arrival_time < gA->arrival_time) {
            if (gB->value == 2) {
                if (gate->value == 0) {
                    ASSIGN_TRUE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(15)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                }
            }
        }

        else if (gB->arrival_time > gA->arrival_time) {
            if (gA->value == 2) {
                ASSIGN_FALSE(gA)

                if (gate->value == 1) {
                    ASSIGN_FALSE(gB)
                }
                else { // gate->value == 0
                    ASSIGN_TRUE(gB)
                }

                PUSH_GATE(gB)
                CALL_FUNCTION(16)
                POP_GATE_2()
                UNDO_ASSIGN(gB)
                UNDO_ASSIGN(gA)
            }
        }

        else { // Both of them have same arrival time.
            if (gate->value == 1) {
                if (gB->value == 2) {
                    ASSIGN_FALSE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(17)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                }
            }

            else { // gate->value == 0
                if (gA->value == 2) {
                    ASSIGN_FALSE(gA)
                    ASSIGN_TRUE(gB)
                    PUSH_GATE(gB)
                    CALL_FUNCTION(18)
                    POP_GATE_2()
                    UNDO_ASSIGN(gB)
                    UNDO_ASSIGN(gA)
                }
            }
        }
    } // else if (gate->module == Module::NOR2)

    else if (gate->module == Module::NOT1) {
        if (gate->froms[0]->value == 2) {
            if (gate->value == 1) {
                ASSIGN_FALSE(gate->froms[0])
            }
            else { // gate->value == 0
                ASSIGN_TRUE(gate->froms[0])
            }

            PUSH_GATE(gate->froms[0])
            CALL_FUNCTION(19)
            POP_GATE_1()
            UNDO_ASSIGN(gate->froms[0])
        }
    } // else if (gate->module == Module::NOT1)

    else if (gate->module == Module::PI) {
        if (slack < slack_constraint) {
            if (solver.solve(assumptions)) {
                paths.push_back(path);

                // Record value of all gate along a path.
                std::vector<bool> path_value(path.size());
                for (size_t i = 0; i < path.size(); ++i) {
                    path_value[i] = path[i]->value;
                }
                values.push_back(path_value);

                // Record input vector.
                InputVec input_vec(cir.primary_inputs.size());
                for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
                    input_vec[i] = 
                        toInt(solver.model[cir.primary_inputs[i]->var]) ^ 1;
                }
                input_vecs.push_back(input_vec);
            }
        }
    }

    else {
        assert(false && "Error: Unknown gate type.\n");
    }

    goto pop_function;

    case 0: 
        ; // Function returned to root caller.

    } // switch (point)
}

int Sta::Ana::find_sensitizable_paths(
    Cir::Circuit&                     cir,
    int                               time_constraint,
    int                               slack_constraint,
    std::vector<Cir::Path>&           paths,
    std::vector< std::vector<bool> >& values,
    std::vector<Cir::InputVec>&       input_vecs) {

    // Calculate arrival time.
    calculate_arrival_time(cir);

    // Initialize gate value to undefined state.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        cir.primary_inputs[i]->value = 2;
    }
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->value = 2;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->value = 2;
    }

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
        case Cir::Module::NAND2:
            success = add_NAND_clause(gate->froms[0]->var,
                                      gate->froms[1]->var,
                                      gate->var,
                                      solver);
            break;

        case Cir::Module::NOR2:
            success = add_NOR_clause(gate->froms[0]->var,
                                     gate->froms[1]->var,
                                     gate->var,
                                     solver);
            break;

        case Cir::Module::NOT1:
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
    if (!solver.simplify()) {
        std::cerr << "Error: Clause conflicts while simplifying.\n";
        return 1;
    }

    // Find sensitizable path.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Cir::Gate* po = cir.primary_outputs[i];
        trace(po, cir, time_constraint, slack_constraint,
              solver, paths, values, input_vecs);
    }

    return 0;
}
