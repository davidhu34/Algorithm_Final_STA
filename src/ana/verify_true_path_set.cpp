#include "sta/src/ana/analyzer.h"

#include <iostream>
#include <queue>

#ifndef NDEBUG
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;
#endif

// Calculate value and arrival time of all gates. Please initialize 
// PI to required value and all other gate to undefined value before
// passing circuit to me.
//
static void calculate_value_and_arrival_time(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;
    using Sta::Cir::Module;

    // This is the front wave. Every gate inside queue has known 
    // arrival time. This queue should be sorted according to
    // arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* pi         = cir.primary_inputs[i];
        pi->arrival_time = 0;
        q.push(pi);
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        // Let all of gate's fan-out that does not in queue nor
        // visited check their inputs' value and arrival time. If
        // a fan-out decide that it knows its value and arrival
        // time now, add it into queue.
        //
        for (size_t i = 0; i < gate->tos.size(); ++i) {
            Gate* fanout = gate->tos[i];
            
            if (fanout->value == 2) { // Not in queue nor visited.
                switch (fanout->module) {
                case Module::NAND2:
                #define FANOUT_CHECK_ITS_INPUT(v0, v1)                    \
                    if (gate->value == v0) {                              \
                        fanout->value        = v1;                        \
                        fanout->arrival_time = gate->arrival_time + 1;    \
                        q.push(fanout);                                   \
                    }                                                     \
                    else { /* gate->value == 1 */                         \
                        if (fanout->froms[0]->value != 2 &&               \
                            fanout->froms[1]->value != 2   ) {            \
                                                                          \
                            assert(fanout->froms[0]->value == v1);        \
                            assert(fanout->froms[1]->value == v1);        \
                                                                          \
                            fanout->value        = v0;                    \
                            fanout->arrival_time = gate->arrival_time + 1;\
                            q.push(fanout);                               \
                        }                                                 \
                    }

                    FANOUT_CHECK_ITS_INPUT(0, 1)
                    break;

                case Module::NOR2:
                    FANOUT_CHECK_ITS_INPUT(1, 0)
                    break;

                case Module::NOT1:
                    fanout->value        = !gate->value;
                    fanout->arrival_time = gate->arrival_time + 1;
                    q.push(fanout);
                    break;

                case Module::PO:
                    fanout->value        = gate->value;
                    fanout->arrival_time = gate->arrival_time;
                    break;
                }
            }
        } // for each fanout of gate
    } // while (!q.empty())
}

#define ASSERT(condition)                                                 \
    if (!(condition)) {                                                   \
        std::cerr << "Error: Gate '" << g1->name << "' in Path { "        \
                  << path_idx + 1 << " } is not part of true path."       \
                  << "\nnext->name  = " << next->name                     \
                  << "\nnext->module= " << cir.modules[next->module].name \
                  << "\nnext->value = " << (int)(next->value)             \
                  << "\nnext->a_time= " << next->arrival_time             \
                  << "\ng1->name    = " << g1->name                       \
                  << "\ng1->value   = " << (int)(g1->value)               \
                  << "\ng1->a_time  = " << g1->arrival_time               \
                  << "\ng2->name    = " << g2->name                       \
                  << "\ng2->value   = " << (int)(g2->value) << "\n";      \
                  << "\ng2->a_time  = " << g1->arrival_time               \
        return 1;                                                         \
    }

// Verify a whether a path is a true path. Return 0 if it is true.
static int verify_true_path(Sta::Cir::Circuit&        cir,
                            const Sta::Cir::Path&     path,
                            size_t                    path_idx,
                            const std::vector<bool>&  path_value,
                            const Sta::Cir::InputVec& input_vec  ) {
    using Sta::Cir::Gate;
    using Sta::Cir::Module;
    
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        cir.primary_inputs[i]->value = input_vec[i];
    }
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->value = 2;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->value = 2;
    }

    calculate_value(cir);

    for (size_t i = 0; i < path.size() - 1; ++i) {
        const Gate* g1   = path[i];
        const Gate* next = path[i + 1];
        const Gate* g2   = 0;

        switch (next->module) {
        case Module::NAND2: {
        #define CHECK_GATE(v0, v1, afirst, bfirst)               \
            /* Try to find side input */                         \
            if (next->froms[0] == g1) {                          \
                g2 = next->froms[1];                             \
            }                                                    \
            else {                                               \
                g2 = next->froms[0];                             \
            }                                                    \
                                                                 \
            uint8_t Y = next->value;                             \
            uint8_t A = g1->value;                               \
            uint8_t B = g2->value;                               \
            int     d = g1->arrival_time - g2->arrival_time;     \
            /* d >  0: g2 arrived first.      */                 \
            /* d <  0: g1 arrived first.      */                 \
            /* d == 0: they arrived together. */                 \
                                                                 \
            ASSERT((Y ==  0 && A ==  1 && B ==  1 && bfirst) ||  \
                   (Y ==  0 && A ==  1 && B ==  1 && d == 0) ||  \
                   (Y ==  1 && A ==  0 && B ==  0 && afirst) ||  \
                   (Y ==  1 && A ==  0 && B ==  0 && d == 0) ||  \
                   (Y == v1 && A == v0 && B == v1 && d <  0) ||  \
                   (Y == v1 && A == v0 && B == v1 && d >  0) ||  \
                   (Y == v1 && A == v0 && B == v1 && d == 0)   )
            
            CHECK_GATE(0, 1, d < 0, d > 0)
            break;
        }
        case Module::NOR2: {
            CHECK_GATE(1, 0, d > 0, d < 0)
            break;
        }
        } // switch
        
        if (g1->value != path_value[i]) {
            std::cerr << "Error: Gate '" << g1->name << "' value "
                      << "does not match with mine.\n";
            return 1;
        }
    }
    return 0;
}

int Sta::Ana::verify_true_path_set(
    Cir::Circuit&                           cir,
    int                                     time_constraint,
    int                                     slack_constraint,
    const std::vector<Cir::Path>&           paths,
    const std::vector< std::vector<bool> >& values,
    const std::vector<Cir::InputVec>&       input_vecs) {

    #ifndef NDEBUG
    std::cerr << "Number of verified path: ";

    start_time       = 0;
    time_difference  = 0.0;
    time_step        = 1.0;
    buffer           = "";
    #endif

    int return_code = 0;

    for (size_t i = 0; i < paths.size(); ++i) {
        return_code = verify_true_path(cir, 
                                       paths[i], 
                                       i,
                                       values[i], 
                                       input_vecs[i]);

        #ifndef NDEBUG
        time_difference = difftime(time(0), start_time);

        if (time_difference > time_step) {
            time_step = time_difference + 1.0;
            std::cerr << std::string(buffer.size(), '\b');

            buffer = Sta::Util::to_str(i);
            std::cerr << buffer;
        }
        #endif
    }

    #ifndef NDEBUG
    std::cerr << std::string(buffer.size(), '\b');
    std::cerr << paths.size() << "\n";
    #endif

    return return_code;
}
