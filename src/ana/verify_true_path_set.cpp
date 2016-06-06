#include "sta/src/ana/analyzer.h"

#include <iostream>
#include <queue>

// Calculate value of all gates. Please initialize PI to required
// value and all other gate to undefined value before passing
// circuit to me.
//
static void calculate_value(Sta::Cir::Circuit& cir) {
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
                
                // Assign value to that fan-out.
                switch (fanout->module) {
                case Module::NAND2:
                    fanout->value = !(fanout->froms[0]->value &&
                                      fanout->froms[1]->value   );
                    break;

                case Module::NOR2:
                    fanout->value = !(fanout->froms[0]->value ||
                                      fanout->froms[1]->value   );
                    break;

                case Module::NOT1:
                    fanout->value = !fanout->froms[0]->value;
                    break;

                case Module::PO:
                    fanout->value = fanout->froms[0]->value;
                    break;
                }

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

#define ASSERT(condition)                                                 \
    if (!(condition)) {                                                   \
        std::cerr << "Error: Gate '" << g1->name << "' in Path { "        \
                  << path_idx + 1 << " } is not part of true path."       \
                  << "\nnext->name  = " << next->name                     \
                  << "\nnext->module= " << cir.modules[next->module].name \
                  << "\nnext->value = " << (int)(next->value)             \
                  << "\ng1->name    = " << g1->name                       \
                  << "\ng1->value   = " << (int)(g1->value)               \
                  << "\ng2->name    = " << g2->name                       \
                  << "\ng2->value   = " << (int)(g2->value) << "\n";      \
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

        if (next->module == Module::NAND2 ||
            next->module == Module::NOR2    ) {

            // Try to find side input
            if (next->froms[0] == g1) {
                g2 = next->froms[1];
            }
            else {
                g2 = next->froms[0];
            }

            if (next->module == Module::NAND2) {
                ASSERT(g1->value == 0 || g2->value == 1)
            }
            else { // next->module == Module::NOR2
                ASSERT(g1->value == 1 || g2->value == 0)
            }
        }


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

    int return_code = 0;

    for (size_t i = 0; i < paths.size(); ++i) {
        return_code = verify_true_path(cir, 
                                       paths[i], 
                                       i,
                                       values[i], 
                                       input_vecs[i]);
    }

    return return_code;
}
