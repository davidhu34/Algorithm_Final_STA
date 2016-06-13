#include "sta/src/ana/analyzer.h"

#include <assert.h>
#include <queue>

void Sta::Ana::calculate_value_and_arrival_time(
    const Cir::Circuit&  cir,
    const Cir::InputVec& input_vec) {

    using Cir::Gate;
    using Cir::Module;
    using Cir::Time::UNKNOWN;

    // Reset gates' value and arrival time.
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Gate* g = cir.primary_outputs[i];
        g->value        = 2;
        g->arrival_time = UNKNOWN;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        Gate* g = cir.logic_gates[i];
        g->value        = 2;
        g->arrival_time = UNKNOWN;
    }

    // This is the front wave. Every gate inside queue has known 
    // arrival time and value. This queue should be sorted according
    // to arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* g = cir.primary_inputs[i];
        g->value        = input_vec[i];
        g->arrival_time = 0;
        q.push(g);
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
                        Gate* gA = fanout->froms[0];                      \
                        Gate* gB = fanout->froms[1];                      \
                                                                          \
                        if (gA->value == v1 && gB->value == v1) {         \
                            fanout->value        = v0;                    \
                            fanout->arrival_time =                        \
                                (gA->arrival_time > gB->arrival_time ?    \
                                 gA->arrival_time : gB->arrival_time  );  \
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

                } // switch (gate->module)

            } // if fanout is not visited

        } // for each fanout of gate

    } // while (!q.empty())
}

