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

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
//
// #### Input
//
// - `po`
// - `cir`
//
// #### Output
//
// Add sensitizable path to `paths` and an input vector that can make
// that path sensitized.
//
static void trace(Sta::Cir::Gate*             po,
                  Sta::Cir::Circuit&          cir,
                  std::vector<Cir::Path>&     paths, 
                  std::vector<Cir::InputVec>& input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using sta::Cir::InputVec;
    using Sta::Cir::Module;
    using Minisat::mkLit;

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

    // Call function
    continue_point.push(0);
    goto start_function;

pop_function:
    point = continue_point.top(); 
    continue_point.pop();

    switch (point) {

start_function:
    gate = path.back();

    if (gate->value == 2) { // Floating
        assert(gate->module == Module::PO);

        gate->value = 0;
        gate->froms[0]->value = 0;
        assumptions.push(mkLit(gate->var, 1));
        path.push_back(gate->froms[0]);

        // Call function.
        continue_point.push(1);
        goto start_function;
    case 1:

        path.pop_back();
        gate = path.back();
        assumptions.pop();
        gate->froms[0]->value = 2;
        gate->value = 2;

        gate->value = 1;
        gate->froms[0]->value = 1;
        assumptions.push(mkLit(gate->var));
        path.push_back(gate->froms[0]);

        // Call function
        continue_point.push(2);
        goto start_function;
    case 2:

        path.pop_back();
        gate = path.back();
        assumptions.pop();
        gate->froms[0]->value = 2;
        gate->value = 2;
    } // if (gate->value == 2) Floating

    else if (gate->module == Module::NAND) {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gA become a true path.
        // new_code_block(nand_make_from_a_true_path)
        
        if (gA->arrival_time < gB->arrival_time) {
            if (gA->value == 2) { // Hasn't assign any value.
                if (gate->value == 1) {
                    gA->value = 0;
                    assumptions.push(mkLit(gA->var, 1));
                    path.push_back(gA);

                    // Call function
                    continue_point.push(3);
                    goto start_function;
                case 3:

                    path.pop();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gA->value = 2;
                }
            }
        } // if (gA->arrival_time < gB->arrival_time) 

        else if (gA->arrival_time > gB->arrival_time) {
            if (gB->value == 2) {
                gB->value = 1;
                assumptions.push(mkLit(gB->var))

                if (gate->value == 1) {
                    gA->value = 0;
                    assumptions.push(mkLit(gA->var, 1));
                }
                else { // gate->value == 0
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                }

                path.push_back(gA);

                // Call function
                continue_point.push(4);
                goto start_function;
            case 4:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gA->value = 2;
                assumptions.pop();
                gB->value = 2;
            }
        } // else if (gA->arrival_time > gB->arrival_time) 

        else { // Both of them have same arrival time.
            if (gA->value == 2) {
                if (gate->value == 1) {
                    gA->value = 0;
                    assumptions.push(mkLit(gA->var, 1));
                    path.push_back(gA);

                    // Call function
                    continue_point.push(5);
                    goto start_function;
                case 5:

                    path.pop_back();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gA->value = 2;
                }
            }

            if (gB->value == 2) {
                gB->value = 1;
                assumptions.push(gB->var);

                if (gate->value == 1) {
                    gA->value = 0;
                    assumptions.push(mkLit(gA->var, 1));
                }
                else { // gate->value == 0
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                }

                path.push_back(gA);

                // Call function
                continue_point.push(6);
                goto start_function;
            case 6:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gA->value = 2;
                assumptions.pop();
                gB->value = 2;
            }
        } // else // Both of them have same arrival time.

        // end_code_block(nand_make_from_a_true_path)

        // Try to make gB become a true path.
        //
        // 1. Take code block nand_make_from_a_true_path.
        // 2. Swap "gA->" and "gB->"
        // 3. Replace "path.push_back(gA)" with "path.push_back(gB)"
        //
        // new_code_block(nand_make_from_b_true_path)

        if (gB->arrival_time < gA->arrival_time) {
            if (gB->value == 2) { // Hasn't assign any value.
                if (gate->value == 1) {
                    gB->value = 0;
                    assumptions.push(mkLit(gB->var, 1));
                    path.push_back(gB);

                    // Call function
                    continue_point.push(7);
                    goto start_function;
                case 7:

                    path.pop();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gB->value = 2;
                }
            }
        } // if (gB->arrival_time < gA->arrival_time)

        else if (gB->arrival_time > gA->arrival_time) {
            if (gA->value == 2) {
                gA->value = 1;
                assumptions.push(mkLit(gA->var))

                if (gate->value == 1) {
                    gB->value = 0;
                    assumptions.push(mkLit(gB->var, 1));
                }
                else { // gate->value == 0
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                }

                path.push_back(gB);

                // Call function
                continue_point.push(8);
                goto start_function;
            case 8:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gB->value = 2;
                assumptions.pop();
                gA->value = 2;
            }
        } // else if (gB->arrival_time > gA->arrival_time)

        else { // Both of them have same arrival time.
            if (gB->value == 2) {
                if (gate->value == 1) {
                    gB->value = 0;
                    assumptions.push(mkLit(gB->var, 1));
                    path.push_back(gB);

                    // Call function
                    continue_point.push(9);
                    goto start_function;
                case 9:

                    path.pop_back();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gB->value = 2;
                }
            }

            if (gA->value == 2) {
                gA->value = 1;
                assumptions.push(gA->var);

                if (gate->value == 1) {
                    gB->value = 0;
                    assumptions.push(mkLit(gB->var, 1));
                }
                else { // gate->value == 0
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                }

                path.push_back(gB);

                // Call function
                continue_point.push(10);
                goto start_function;
            case 10:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gB->value = 2;
                assumptions.pop();
                gA->value = 2;
            }
        } // else Both of them have same arrival time.

        // end_code_block(nand_make_from_b_true_path)

    } // if (gate->module == Module::NAND)

    else if (gate->module == Module::NOR) {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gate->froms[0] become a true path.
        //
        // 1. Take code block nand_make_from_a_true_path.
        // 2. Swap "mkLit(gA->var, 1)" and "mkLit(gA->var)".
        // 3. Swap "mkLit(gB->var, 1)" and "mkLit(gB->var)".
        // 4. Swap "value = 0" and "value = 1".
        // 
        // new_code_block(nor_make_from_a_true_path)
        
        if (gA->arrival_time < gB->arrival_time) {
            if (gA->value == 2) { // Hasn't assign any value.
                if (gate->value == 1) {
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                    path.push_back(gA);

                    // Call function
                    continue_point.push(11);
                    goto start_function;
                case 11:

                    path.pop();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gA->value = 2;
                }
            }
        } // if (gA->arrival_time < gB->arrival_time) 

        else if (gA->arrival_time > gB->arrival_time) {
            if (gB->value == 2) {
                gB->value = 0;
                assumptions.push(mkLit(gB->var, 1))

                if (gate->value == 1) {
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                }
                else { // gate->value == 0
                    gA->value = 0;
                    assumptions.push(mkLit(gA-var, 1));
                }

                path.push_back(gA);

                // Call function
                continue_point.push(12);
                goto start_function;
            case 12:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gA->value = 2;
                assumptions.pop();
                gB->value = 2;
            }
        } // else if (gA->arrival_time > gB->arrival_time)

        else { // Both of them have same arrival time.
            if (gA->value == 2) {
                if (gate->value == 1) {
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                    path.push_back(gA);

                    // Call function
                    continue_point.push(13);
                    goto start_function;
                case 13:

                    path.pop_back();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gA->value = 2;
                }
            }

            if (gB->value == 2) {
                gB->value = 0;
                assumptions.push(gB->var);

                if (gate->value == 1) {
                    gA->value = 1;
                    assumptions.push(mkLit(gA->var));
                }
                else { // gate->value == 0
                    gA->value = 0;
                    assumptions.push(mkLit(gA-var, 1));
                }

                path.push_back(gA);

                // Call function
                continue_point.push(14);
                goto start_function;
            case 14:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gA->value = 2;
                assumptions.pop();
                gB->value = 2;
            }
        } // else // Both of them have same arrival time.

        // end_code_block(nor_make_from_a_true_path)

        // Try to make gate->froms[1] become a true path
        //
        // 1. Take code block nor_make_from_a_true_path.
        // 2. Swap "gA->" and "gB->".
        // 3. Replace "path.push_back(gA)" with "path.push_back(gB)".
        // 4. Renew continue_point.
        //
        // new_code_block(nor_make_from_b_true_path)
        
        if (gB->arrival_time < gA->arrival_time) {
            if (gB->value == 2) { // Hasn't assign any value.
                if (gate->value == 1) {
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                    path.push_back(gB);

                    // Call function
                    continue_point.push(15);
                    goto start_function;
                case 15:

                    path.pop();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gB->value = 2;
                }
            }
        } // if (gB->arrival_time < gA->arrival_time)

        else if (gB->arrival_time > gA->arrival_time) {
            if (gA->value == 2) {
                gA->value = 0;
                assumptions.push(mkLit(gA->var, 1))

                if (gate->value == 1) {
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                }
                else { // gate->value == 0
                    gB->value = 0;
                    assumptions.push(mkLit(gA-var, 1));
                }

                path.push_back(gB);

                // Call function
                continue_point.push(16);
                goto start_function;
            case 16:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gB->value = 2;
                assumptions.pop();
                gA->value = 2;
            }
        } // else if (gB->arrival_time > gA->arrival_time)

        else { // Both of them have same arrival time.
            if (gB->value == 2) {
                if (gate->value == 1) {
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                    path.push_back(gB);

                    // Call function
                    continue_point.push(17);
                    goto start_function;
                case 17:

                    path.pop_back();
                    gate = path.back();
                    gA = gate->froms[0];
                    gB = gate->froms[1];
                    assumptions.pop();
                    gB->value = 2;
                }
            }

            if (gA->value == 2) {
                gA->value = 0;
                assumptions.push(gA->var);

                if (gate->value == 1) {
                    gB->value = 1;
                    assumptions.push(mkLit(gB->var));
                }
                else { // gate->value == 0
                    gB->value = 0;
                    assumptions.push(mkLit(gA-var, 1));
                }

                path.push_back(gB);

                // Call function
                continue_point.push(18);
                goto start_function;
            case 18:

                path.pop_back();
                gate = path.back();
                gA = gate->froms[0];
                gB = gate->froms[1];
                assumptions.pop();
                gB->value = 2;
                assumptions.pop();
                gA->value = 2;
            }
        } // else Both of them have same arrival time.

        // end_code_block(nor_make_from_b_true_path)

    } // else if (gate->module == Module::NOR)

    else if (gate->module == Module::NOT) {
        gA = gate->froms[0];

        if (gA->value == 2) {
            if (gate->value == 1) {
                gA->value = 0;
                assumptions.push(mkLit(gA->var, 1));
            }
            else { // gate->value == 0
                gA->value = 1;
                assumptions.push(mkLit(gA->var));
            }

            path.push_back(gA);

            // Call function
            continue_point.push(19);
            goto start_function;
        case 19:

            path.pop_back();
            gate = path.back();
            gA = gate->froms[0];
            assumptions.pop();
            gA->value = 2;
        }
    } // else if (gate->module == Module::NOT)

    else if (gate->module == Module::PI && solver.solve(assumptions)) {
        paths.push_back(path);

        InputVec input_vec(cir.primary_inputs.size());
        for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
            input_vec[i] = solver.model[cir.primary_inputs[i]->var] ^ 1;
        }

        input_vecs.push_back(input_vec);
    }

    else {
        assert(false, "Error: Unknown gate type.\n");
    }

    goto pop_function;

    case 0: 
        ; // Function returned to root caller.

    } // switch (point)
}

int Sta::Ana::find_sensitizable_paths(
    Cir::Circuit&               cir,
    int                         time_constraint,
    int                         slack_constraint,
    std::vector<Cir::Path>&     paths,
    std::vector<Cir::InputVec>& input_vecs) {

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
    if (!solver.simplify()) {
        std::cerr << "Error: Clause conflicts while simplifying.\n";
        return 1;
    }

    // Find sensitizable path.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Cir::Gate* po = cir.primary_outputs[i];

        int slack = time_constraint - po->arrival_time;
        if (po->arrival_time < time_constraint &&
            slack            < slack_constraint  ) {
            
            trace(po, cir, paths, input_vecs, assumptions, path);
        }
    }

    return 0;
}
