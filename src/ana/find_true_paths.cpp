#include "sta/src/ana/analyzer.h"

#include <assert.h>
#include <stack>

#include "minisat/src/core/Solver.h"
#include "sta/src/ana/helper.h"

#ifndef NDEBUG
#include <time.h>
#include <iostream>
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;
#endif

#define RESET_GATE_VALUE_FOR_GROUP(group)                            \
    for (size_t i = 0; i < cir.group.size(); ++i) {                  \
        cir.group[i]->value = 2;                                     \
    }

static void reset_gate_value(const Sta::Cir::Circuit& cir) {
    using Sta::Cir::Time::UNKNOWN;
    using Sta::Cir::Gate;

    RESET_GATE_VALUE_FOR_GROUP(primary_inputs)
    RESET_GATE_VALUE_FOR_GROUP(primary_outputs)
    RESET_GATE_VALUE_FOR_GROUP(logic_gates)
}

static void reset_gate_value(const Sta::Cir::Circuit&   cir,
                             const Sta::Cir::Path&      path,
                             const Sta::Cir::PathValue& path_value,
                             const Sta::Cir::Path&      subpath,
                             const Sta::Cir::PathValue& subpath_value) {
    reset_gate_value(cir);

    for (size_t i = 0; i < path.size(); ++i) {
        path[i]->value = path_value[i];
    }
    for (size_t i = 0; i < subpath.size(); ++i) {
        subpath[i]->value = subpath_value[i];
    }
}

// Call function and create a return point. `n` is the return point
// index. It must be a literal number without any operation. Each
// return point index must be unique. Please do not put semicolon
// after calling this function.
//
#define CALL_FUNCTION(n)      \
    continue_point.push(n);   \
    goto start_function;      \
    lbl_ ## n:

// Perform code substitution. Please do not put semicolon after
// calling these functions.
//
// CAUTION: SURROUND THEM WITH BRACES WHEN USE WITH BRANCH OR LOOP.
//
#define PUSH_PATH(gate)                    \
    path.push_back(gate);                  \
    path_value.push_back(gate->value);     \
    slack -= 1;

#define POP_PATH_1()                       \
    slack += 1;                            \
    path_value.pop_back();                 \
    path.pop_back();                       \
    gate = path.back();                    

#define POP_PATH_2()                       \
    slack += 1;                            \
    path_value.pop_back();                 \
    path.pop_back();                       \
    gate = path.back();                    \
    gA = gate->froms[0];                   \
    gB = gate->froms[1];                   

#define PUSH_SUBPATH(gate)                 \
    subpath.push_back(gate);               \
    subpath_value.push_back(gate->value);  \

#define POP_SUBPATH()                      \
    subpath_value.pop_back();              \
    subpath.pop_back();                    \

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
// It will fill `paths`, `values` and `input_vector`.
//
static void trace(Sta::Cir::Gate*                   po,
                  const Sta::Cir::Circuit&          cir,
                  int                               time_constraint,
                  int                               slack_constraint,
                  Minisat::Solver&                  solver,
                  std::vector<Sta::Cir::Path>&      paths, 
                  std::vector<Sta::Cir::PathValue>& values,
                  std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using Sta::Cir::PathValue;
    using Sta::Cir::InputVec;
    using Sta::Cir::Module;
    using Sta::Ana::no_conflict;

    // Try to mimic recursive function call without using function.
    // Use continue_point and switch statement to make sure function
    // return to correct point.
    std::stack<int> continue_point;

    // Static variable of that fake function.
    Path      path(1, po);
    PathValue path_value;
    Path      subpath;
    PathValue subpath_value;

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
    #define CASE(n) \
        case n: goto lbl_ ## n; break;

        CASE(0)
        CASE(1)
        CASE(2)
        CASE(3)
        CASE(4)
        CASE(5)
        CASE(6)
        CASE(7)
        CASE(8)
        CASE(9)
        CASE(10)
        CASE(11)

        default:
            assert(false && "Return point unknown.");
            break;
    } // switch (point)

start_function:
    gate = path.back();

    if (gate->min_arrival_time > slack + 1) {
        goto pop_function;
    }

    if (slack == -1 && gate->module != Module::PI) {
        goto pop_function;
    }

    switch (gate->module) {
    case Module::PO: {
        assert(gate->value == 2); // Undefined

        gate->value = 0;
        path_value.push_back(0);
        gate->froms[0]->value = 0;
        PUSH_PATH(gate->froms[0])
        CALL_FUNCTION(1)
        POP_PATH_1()
        gate->froms[0]->value = 2;
        path_value.pop_back();
        gate->value = 2;

        gate->value = 1;
        path_value.push_back(1);
        gate->froms[0]->value = 1;
        PUSH_PATH(gate->froms[0])
        CALL_FUNCTION(2)
        POP_PATH_1()
        gate->froms[0]->value = 2;
        path_value.pop_back();
        gate->value = 2;

        break;
    } // PO

    case Module::NAND2: {
        gA = gate->froms[0];
        gB = gate->froms[1];

    #define MAKE_TRUE_PATH(gA, gB, v0, v1, p1, p2)          \
        if (gate->value == v1) {                            \
            if (gA->value == 2) {                           \
                gA->value = v0;                             \
                PUSH_PATH(gA)                               \
                CALL_FUNCTION(p1)                           \
                POP_PATH_2()                                \
                gA->value = 2;                              \
            }                                               \
        }                                                   \
                                                            \
        else { /* gate->value == 0 */                       \
            if (gA->value == 2 && gB->value == 2) {         \
                gA->value = v1;                             \
                gB->value = v1;                             \
                PUSH_SUBPATH(gB)                            \
                PUSH_PATH(gA)                               \
                CALL_FUNCTION(p2)                           \
                POP_PATH_2()                                \
                POP_SUBPATH()                               \
                gB->value = 2;                              \
                gA->value = 2;                              \
            }                                               \
        }                                                   

        // Try to make gA become a true path.
        MAKE_TRUE_PATH(gA, gB, 0, 1, 3, 4)

        // Try to make gB become a true path.
        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 0, 1, 5, 6)

        break;
    } // NAND2

    case Module::NOR2: {
        gA = gate->froms[0];
        gB = gate->froms[1];

        // Try to make gA become a true path.
        MAKE_TRUE_PATH(gA, gB, 1, 0, 7, 8)

        // Try to make gB become a true path
        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 1, 0, 9, 10)

        break;
    } // NOR2

    case Module::NOT1: {
        if (gate->froms[0]->value == 2) {
            gate->froms[0]->value = !gate->value;
            PUSH_PATH(gate->froms[0])
            CALL_FUNCTION(11)
            POP_PATH_1()
            gate->froms[0]->value = 2;
        }
        break;
    } // NOT1

    case Module::PI: {
        
        // slack + 1 is real slack, slack is one less than real slack
        // because of --slack of PI.
        if (slack + 1 < slack_constraint) { 
            if (no_conflict(cir, path, path_value, subpath, 
                            subpath_value)                 ) {
                paths.push_back(path);
                values.push_back(path_value);

                // Record input vector.
                InputVec input_vec(cir.primary_inputs.size());
                for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
                    input_vec[i] = cir.primary_inputs[i]->value;
                }
                input_vecs.push_back(input_vec);

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
            }

            // Restore cir to state before calling no_conflict()
            reset_gate_value(cir, path, path_value, subpath, 
                             subpath_value);
        }
        break;
    } // PI

    default:
        assert(false && "Error: Unknown gate type.\n");
        break;
    } // switch (gate->module)

    goto pop_function;

    lbl_0:
        ; // Function returned to root caller.

}

bool Sta::Ana::find_true_paths(
    const Cir::Circuit&          cir,
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

    calculate_min_arrival_time(cir);
    calculate_max_arrival_time(cir);
    reset_gate_value(cir);

    Minisat::Solver solver;
    bool success = init_solver(cir, solver);
    assert(success);

    // Find true path.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Cir::Gate* po = cir.primary_outputs[i];
        trace(po, cir, time_constraint, slack_constraint,
              solver, paths, values, input_vecs);
    }

    #ifndef NDEBUG
    std::cerr << std::string(buffer.size(), '\b');
    std::cerr << input_vecs.size() << "\n";
    #endif

    return true;
}
