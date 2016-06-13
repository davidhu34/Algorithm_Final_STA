#include "sta/src/ana/analyzer.h"

#include <queue>

void Sta::Ana::calculate_min_arrival_time(const Cir::Circuit& cir) {
    using Cir::Gate;
    using Cir::Module;

    // Assign 0 to all gate's tag.
    // 0: Not yet visited, not in queue.
    // 1: In queue, not visited.
    // 2: Visited, not in queue.
    //
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->tag = 0;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->tag = 0;
    }

    // For all gate inside queue, at least 1 of its fan-in has arrived.
    // Every gate inside queue has already known min arrival time.
    // This queue should be sorted according to min arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* pi = cir.primary_inputs[i];
        pi->min_arrival_time = 0;
        q.push(pi);
        pi->tag = 1;
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        for (size_t i = 0; i < gate->tos.size(); ++i) {
            Gate* fanout = gate->tos[i];
            
            // If fanout is not visited nor in queue
            if (fanout->tag == 0) {
                
                // Assign arrival time to that fan-out.
                if (fanout->module == Module::PO) {
                    fanout->min_arrival_time = gate->min_arrival_time;

                    // No need to put PO into queue, because PO does
                    // not have fan-out.
                    fanout->tag = 2;
                }
                else {
                    fanout->min_arrival_time = gate->min_arrival_time + 1;

                    // Add that fan-out to queue.
                    q.push(fanout);
                    fanout->tag = 1;
                }
            }
        }
    }
}

