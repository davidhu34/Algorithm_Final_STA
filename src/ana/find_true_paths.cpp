#include "sta/src/ana/analyzer.h"

#include <cassert>
#include <ctime>
#include <iostream>
#include <stack>

#include "minisat/src/core/Solver.h"

namespace {

// Variables of SAT solver that can be reused.
struct VarStack {
    size_t           idx;
    std::vector<int> vars;
    Minisat::Solver& solver;

    explicit VarStack(Minisat::Solver& _solver): 
        idx    (0      ),
        vars   (16     ),
        solver (_solver) {

        for (size_t i = 0; i < vars.size(); ++i) {
            vars[i] = solver.newVar();
        }
    }

    int get_var() {
        if (idx == vars.size()) {
            vars.push_back(solver.newVar());
        }
        return vars[idx++];
    }

    void clear() {
        idx = 0;
    }
};

} // namespace

#ifndef NDEBUG
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;
#endif

// Add NAND clause into solver.
// Return True if success, false otherwise.
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
// Return True if success, false otherwise.
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
// Return True if success, false otherwise.
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

// Add a clause to exclude this solution, so that we can get
// another solution.
static void add_clause_to_exclude(
    const Sta::Cir::Circuit&  cir,
    const Sta::Cir::InputVec& input_vec,
    int                       new_var,
    Minisat::Solver&          solver) {

    using Minisat::mkLit;

    Minisat::vec<Minisat::Lit> clause;
    clause.push(mkLit(new_var));

    for (size_t i = 0; i < input_vec.size(); ++i) {
        if (input_vec[i]) {
            clause.push(mkLit(cir.primary_inputs[i]->var, 1));
        }
        else {
            clause.push(mkLit(cir.primary_outputs[i]->var));
        }
    }

    solver.addClause_(clause);
    assert(solver.okay());
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
#define ASSIGN(gate, v)                    \
    gate->value = v;                       \
    assumptions.push(mkLit(gate->var, !v));

#define UNASSIGN(gate)                     \
    assumptions.pop();                     \
    gate->value = 2;

#define PUSH_GATE(gate)                    \
    path.push_back(gate);                  \
    slack -= 1;

#define POP_GATE_1()                       \
    slack += 1;                            \
    path.pop_back();                       \
    gate = path.back();                    

#define POP_GATE_2()                       \
    slack += 1;                            \
    path.pop_back();                       \
    gate = path.back();                    \
    gA = gate->froms[0];                   \
    gB = gate->froms[1];                   

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
// It will fill `paths`, `values` and `input_vector`.
//
static void trace(Sta::Cir::Gate*                   po,
                  Sta::Cir::Circuit&                cir,
                  Sta::Cir::Circuit&                cir2,
                  int                               time_constraint,
                  int                               slack_constraint,
                  Minisat::Solver&                  solver,
                  VarStack&                         var_stack,
                  std::vector<Sta::Cir::Path>&      paths, 
                  std::vector<Sta::Cir::PathValue>& values,
                  std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using Sta::Cir::PathValue;
    using Sta::Cir::InputVec;
    using Sta::Cir::Module;
    using Sta::Ana::verify_true_path;
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

    if (gate->min_arrival_time > slack) {
        goto pop_function;
    }

    if (slack == 0 && gate->module != Module::PI) {
        goto pop_function;
    }

    if (gate->value == 2) { // Floating
        assert(gate->module == Module::PO);

        gate->value = 0;
        ASSIGN(gate->froms[0], 0)
        PUSH_GATE(gate->froms[0])
        CALL_FUNCTION(1)
        POP_GATE_1()
        UNASSIGN(gate->froms[0])
        gate->value = 2;

        gate->value = 1;
        ASSIGN(gate->froms[0], 1)
        PUSH_GATE(gate->froms[0])
        CALL_FUNCTION(2)
        POP_GATE_1()
        UNASSIGN(gate->froms[0])
        gate->value = 2;
    }

    else if (gate->module == Module::NAND2) {
        gA = gate->froms[0];
        gB = gate->froms[1];

    #define MAKE_TRUE_PATH(gA, gB, v0, v1, p1, p2, p3, p4)  \
        if (gate->value == v1) {                            \
            if (gA->value == 2) {                           \
                ASSIGN(gA, v0)                              \
                PUSH_GATE(gA)                               \
                CALL_FUNCTION(p1)                           \
                POP_GATE_2()                                \
                UNASSIGN(gA)                                \
            }                                               \
        }                                                   \
                                                            \
        else { /* gate->value == 0 */                       \
            if (gA->value == 2 && gB->value == 2) {         \
                ASSIGN(gA, v1)                              \
                ASSIGN(gB, v1)                              \
                PUSH_GATE(gA)                               \
                CALL_FUNCTION(p2)                           \
                POP_GATE_2()                                \
                UNASSIGN(gB)                                \
                UNASSIGN(gA)                                \
            }                                               \
        }                                                   

        // Try to make gA become a true path.
        
        MAKE_TRUE_PATH(gA, gB, 0, 1, 3, 4, 5, 6)

        // Try to make gB become a true path.

        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 0, 1, 7, 8, 9, 10)

    } // else if (gate->module == Module::NAND2)

    else if (gate->module == Module::NOR2) {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gA become a true path.
        
        MAKE_TRUE_PATH(gA, gB, 1, 0, 11, 12, 13, 14)

        // Try to make gB become a true path

        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 1, 0, 15, 16, 17, 18)
        
    } // else if (gate->module == Module::NOR2)

    else if (gate->module == Module::NOT1) {
        if (gate->froms[0]->value == 2) {
            if (gate->value == 1) {
                ASSIGN(gate->froms[0], 0)
            }
            else { // gate->value == 0
                ASSIGN(gate->froms[0], 1)
            }

            PUSH_GATE(gate->froms[0])
            CALL_FUNCTION(19)
            POP_GATE_1()
            UNASSIGN(gate->froms[0])
        }
    } // else if (gate->module == Module::NOT1)

    else if (gate->module == Module::PI) {
        if (slack + 1 < slack_constraint) {

            // I will add additional assumption into it.
            // Later I will shrink it back to original size.
            int original_size = assumptions.size();

            while (solver.solve(assumptions)) {

                // Record input vector.
                InputVec input_vec(cir.primary_inputs.size());
                for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
                    input_vec[i] = 
                        !toInt(solver.model[cir.primary_inputs[i]->var]);
                }

                if (verify_true_path(cir2, path, input_vec) == 0) {
                    // Remove unneeded clause
                    for (size_t i = 0; i < var_stack.idx; ++i) {
                        solver.addClause(mkLit(var_stack.vars[i]));
                        assert(solver.okay());
                    }
                    solver.simplify();
                    assert(solver.okay());

                    // Clear var_stack
                    var_stack.clear();

                    // Add path to true path list.
                    paths.push_back(path);
                    input_vecs.push_back(input_vec);

                    // Record value of all gate along a path.
                    PathValue path_value(path.size());
                    for (size_t i = 0; i < path.size(); ++i) {
                        path_value[i] = path[i]->value;
                    }
                    values.push_back(path_value);

                    // Output number of found true path.
                    #ifndef NDEBUG
                    time_difference = difftime(time(0), start_time);

                    if (time_difference > time_step) {
                        time_step = time_difference + 1.0;
                        std::cerr << std::string(buffer.size(), '\b');

                        buffer = Sta::Util::to_str(input_vecs.size());
                        std::cerr << buffer;
                    }
                    #endif

                    break;
                }
                else {
                    // Try to get next possible answer.
                    int var = var_stack.get_var();
                    add_clause_to_exclude(cir, input_vec, var, solver);
                    assumptions.push(mkLit(var, 1));
                }
            }

            // Shrink back to original size.
            // Since Lit is POD, no need to call its destructor.
            assumptions.shrink(assumptions.size() - original_size);
            std::cerr << "ha";
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

int Sta::Ana::find_true_paths(
    Cir::Circuit&                cir,
    int                          time_constraint,
    int                          slack_constraint,
    std::vector<Cir::Path>&      paths,
    std::vector<Cir::PathValue>& values,
    std::vector<Cir::InputVec>&  input_vecs) {

    #ifndef NDEBUG
    std::cerr << "Number of true path found: ";

    start_time      = time(0);
    time_difference = 0.0;
    time_step       = 1.0;
    buffer          = "";
    #endif

    // Calculate minimum arrival time.
    calculate_min_arrival_time(cir);

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

    Cir::Circuit cir2     (cir);
    VarStack     var_stack(solver);

    // Find true path.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Cir::Gate* po = cir.primary_outputs[i];
        trace(po, cir, cir2, time_constraint, slack_constraint,
              solver, var_stack, paths, values, input_vecs);
    }

    #ifndef NDEBUG
    std::cerr << std::string(buffer.size(), '\b');
    std::cerr << input_vecs.size() << "\n";
    #endif

    return 0;
}
