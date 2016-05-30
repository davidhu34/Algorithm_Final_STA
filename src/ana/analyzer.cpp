#include "sta/src/ana/analyzer.h"

#include <queue>

void Sta::Ana::assign_arrival_time(Cir::Circuit& cir) {

    // Assign 0 to all gate's tag. It means the number of fan-in of
    // that gate which has arrived.
    //
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->tag = 0;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->tag = 0;
    }

    // Queue store gates that all of its fan-in has arrived.
    std::queue<Cir::Gate*> q;
    
    // Add primary_inputs into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Cir::Gate* pi = cir.primary_inputs[i];
        q.push(pi);
    }

    // While queue is not empty
    while (!q.empty()) {
        Cir::Gate* gate = q.front();
        q.pop();
        
        // Tell gate's fan-outs that it has arrived.
        for (size_t i = 0; i < gate->tos.size(); ++i) {
            gate->tos[i]->tag += 1;
            
            // If that fan-out's fan-ins have all arrived
            if (gate->tos[i]->tag == gate->froms.size()) {
                
                // Add that fan-out to queue.
                q.push(gate->tos[i]);
            }
        }

        // Assign arrival time to gate.
        if (gate->module == Cir::Module::PI) {
            gate->arrival_time = 0;
        }
        else {
            gate->arrival_time = latest_input_arrival_time(gate) + 1;
        }
    }
}

int Sta::Ana::find_sensitizable_paths(
    Cir::Circuit&               circuit,
    std::vector<Cir::Path>&     paths,
    std::vector<Cir::InputVec>& input_vecs) {

    return 0;
}
