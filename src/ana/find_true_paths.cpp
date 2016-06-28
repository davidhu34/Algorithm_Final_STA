#include "sta/src/ana/analyzer.h"

#include <assert.h>
#include <time.h>
#include <iostream>
#include <stack>
#include <string>

#include "sta/src/util/converter.h"

// Timing variables.
static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;

static void reset_timing_variables() {
    start_time      = time(0);
    time_difference = 0.0;
    time_step       = 1.0;
    buffer          = "";
}

static void update_number_of_true_path_found(size_t n, bool force) {
    time_difference = difftime(time(0), start_time);

    if (force || time_difference > time_step) {
        time_step = time_difference + 1.0;
        std::cerr << std::string(buffer.size(), '\b');

        buffer = Sta::Util::to_str(n);
        std::cerr << buffer;
    }
}

#define RESET_GATE_VALUE(group)                     \
    for (size_t i = 0; i < cir.group.size(); ++i) { \
        cir.group[i]->value = 2;                    \
    }

static void reset_gate_value(const Sta::Cir::Subcircuit& cir) {
    RESET_GATE_VALUE(primary_inputs)
    RESET_GATE_VALUE(primary_outputs)
    RESET_GATE_VALUE(logic_gates)
}

static void add_record(const Sta::Cir::PathCopy&         path_copy,
                       const Sta::Cir::InputVec&         input_vec,
                       std::vector<Sta::Cir::Path>&      paths, 
                       std::vector<Sta::Cir::PathValue>& values,
                       std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using Sta::Cir::PathValue;

    PathValue path_value(path_copy.size());
    for (size_t i = 0; i < path_copy.size(); ++i) {
        assert(path_copy[i]->value == 0 || path_copy[i]->value == 1);
        path_value[i] = path_copy[i]->value;
    }

    Path path(path_copy.size());
    for (size_t i = 0; i < path_copy.size(); ++i) {
        path[i] = const_cast<Gate*>(path_copy[i]->orig);
    }

    paths.push_back(path);
    values.push_back(path_value);
    input_vecs.push_back(input_vec);
}

static void restore_temp_var(const Sta::Cir::PathCopy& path, 
                             Sta::Cir::GateCopy*&      gate,
                             Sta::Cir::GateCopy*&      gA, 
                             Sta::Cir::GateCopy*&      gB   ) {
    using Sta::Cir::Module;

    gate = path.back();

    switch (gate->orig->module) {
    case Module::NAND2:
    case Module::NOR2:
        gB = gate->froms[1];
    case Module::PO:
    case Module::NOT1:
        gA = gate->froms[0];
    }
}

// Call function and create a return point. `n` is the return point
// index. It must be a literal number without any operation. Each
// return point index must be unique. Please do not put semicolon
// after calling this function.
//
#define CALL_FUNCTION(g, n)               \
    path.push_back(g);                    \
    restore_temp_var(path, gate, gA, gB); \
                                          \
    continue_point.push(n);               \
    goto start_function;                  \
    lbl_ ## n:

#define CHECK_TIME_CONSTRAINT_VIOLATION()                \
    if (gate->orig->min_arrival_time +                   \
        static_cast<int>(path.size()) >                  \
        time_constraint + 2                         ) {  \
                                                         \
        /* Impossible to arrive at PI in time through */ \
        /* this gate.                                 */ \
        goto pop_function;                               \
    }                                                    \
    if (static_cast<int>(path.size()) ==                 \
        time_constraint + 2           &&                 \
        gate->orig->module != Module::PI) {              \
        /* Unable to reach PI within time_constraint. */ \
        goto pop_function;                               \
    }

#define TRY_ASSIGN_TO_AND_TRACE(g, v, point)       \
    if (g->value == 2) {                           \
        g->value = v;                              \
        CALL_FUNCTION(g, point)                    \
        g->value = 2;                              \
    }

#define TRY_ASSIGN_TO_BOTH_AND_TRACE(g1, g2, v, point) \
    if (g1->value == 2 && g2->value == 2) {            \
        g1->value = v;                                 \
        g2->value = v;                                 \
        subpath.push_back(g2);                         \
        CALL_FUNCTION(g1, point)                       \
        subpath.pop_back();                            \
        g2->value = 2;                                 \
        g1->value = 2;                                 \
    }

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
// It will fill `paths`, `values` and `input_vector`.
//
static void trace(const Sta::Cir::Subcircuit&       cir,
                  int                               time_constraint,
                  int                               slack_constraint,
                  std::vector<Sta::Cir::Path>&      paths, 
                  std::vector<Sta::Cir::PathValue>& values,
                  std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::GateCopy;
    using Sta::Cir::PathCopy;
    using Sta::Cir::PathValue;
    using Sta::Cir::InputVec;
    using Sta::Cir::Module;
    using Sta::Ana::no_conflict_2;

    // Try to mimic recursive function call without using function.
    // Use continue_point and switch statement to make sure function
    // return to correct point.
    std::stack<int> continue_point;

    // Record modified variables so that later I can restore them.
    PathCopy  path;
    PathCopy  subpath;

    // Temporary/helper variables.
    assert(cir.primary_outputs.size() == 1);

    GateCopy* gate = cir.primary_outputs[0];
    GateCopy* gA;
    GateCopy* gB;
    int       point;

    gate->value = 0;
    CALL_FUNCTION(gate, 0)
    gate->value = 1;
    CALL_FUNCTION(gate, 1)
    gate->value = 2;

    return;

pop_function:
    point = continue_point.top(); 
    continue_point.pop();
    path.pop_back();

    if (point > 1) {
        restore_temp_var(path, gate, gA, gB);
    }

    switch (point) {
    case  0: goto lbl_0;  break;
    case  1: goto lbl_1;  break;
    case  2: goto lbl_2;  break;
    case  3: goto lbl_3;  break;
    case  4: goto lbl_4;  break;
    case  5: goto lbl_5;  break;
    case  6: goto lbl_6;  break;
    case  7: goto lbl_7;  break;
    case  8: goto lbl_8;  break;
    case  9: goto lbl_9;  break;
    case 10: goto lbl_10; break;
    case 11: goto lbl_11; break;
    default:
        assert(false && "Return point unknown.");
    } // switch (point)

start_function:
    CHECK_TIME_CONSTRAINT_VIOLATION()

    assert(gate->value == 0 || gate->value == 1);

    switch (gate->orig->module) {
    case Module::PO:
        TRY_ASSIGN_TO_AND_TRACE(gA, gate->value, 2)
        break;

    case Module::NAND2:
        if (gate->value == 1) {
            TRY_ASSIGN_TO_AND_TRACE(gA, 0, 3)
            TRY_ASSIGN_TO_AND_TRACE(gB, 0, 4)
        }
        else { // gate->value == 0
            TRY_ASSIGN_TO_BOTH_AND_TRACE(gA, gB, 1, 5)
            TRY_ASSIGN_TO_BOTH_AND_TRACE(gB, gA, 1, 6)
        }
        break;

    case Module::NOR2:
        if (gate->value == 1) {
            TRY_ASSIGN_TO_BOTH_AND_TRACE(gA, gB, 0, 7)
            TRY_ASSIGN_TO_BOTH_AND_TRACE(gB, gA, 0, 8)
        }
        else { // gate->value == 0
            TRY_ASSIGN_TO_AND_TRACE(gA, 1, 9)
            TRY_ASSIGN_TO_AND_TRACE(gB, 1, 10)
        }
        break;

    case Module::NOT1:
        TRY_ASSIGN_TO_AND_TRACE(gA, !gate->value, 11)
        break;

    case Module::PI: {
        int slack = time_constraint - (path.size() - 2);

        if (slack < slack_constraint) { 
            InputVec input_vec(cir.orig.primary_inputs.size());

            if (no_conflict_2(cir, path, subpath, input_vec)) {
                add_record(path, input_vec,
                           paths, values, input_vecs);
                
                update_number_of_true_path_found(input_vecs.size(), 0);
            }
        }
        break;
    }

    default:
        assert(false && "Error: Unknown gate type.\n");
    } // switch (gate->module)

    goto pop_function;
}

bool Sta::Ana::find_true_paths(
    const Cir::Circuit&          cir,
    int                          time_constraint,
    int                          slack_constraint,
    std::vector<Cir::Path>&      paths,
    std::vector<Cir::PathValue>& values,
    std::vector<Cir::InputVec>&  input_vecs) {

    reset_timing_variables();
    std::cerr << "Number of true path found: ";

    calculate_min_arrival_time(cir);
    calculate_max_arrival_time(cir);

    // Find true path.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Cir::Subcircuit subcir(cir, cir.primary_outputs[i]);
        reset_gate_value(subcir);

        trace(subcir, time_constraint, slack_constraint, 
              paths, values, input_vecs);
    }

    update_number_of_true_path_found(input_vecs.size(), 1);
    std::cerr << "\n";

    return true;
}

