#include "sta/src/ana/analyzer.h"

#include <iostream>
#include <queue>

#ifndef NDEBUG
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer = " ";
#endif

// Calculate _value of all gates. Please initialize PI to required
// _value and all other gate to undefined _value before passing
// circuit to me.
//
static void calculate_value(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;

    // Assign 0 to all gate's _tag. It means the number of fan-in of
    // that gate which has arrived.
    //
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        cir.getOutputs()[i]->_tag = 0;
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_tag = 0;
    }

    // For all gate inside queue, all of its fan-in has arrived.
    // Every gate inside queue has already known arrival time.
    // This queue should be sorted according to arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    typedef std::map<std::string, Gate*>::iterator Iter;

    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it                               ) {

        Gate* pi = it->second;
        pi->_arrivalTime = 0;
        q.push(pi);
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        // Tell gate's fan-outs that it has arrived.
        for (size_t i = 0; i < gate->getFanOut().size(); ++i) {
            Gate* fanout = gate->getFanOut()[i];
            fanout->_tag += 1;
            
            // If that fan-out's fan-ins have all arrived
            if (fanout->_tag == fanout->getFanIn().size()) {
                
                // Assign _value to that fan-out.
                if (fanout->getModel() == "NAND2") {
                    fanout->_value = !(fanout->getFanIn()[0]->_value &&
                                      fanout->getFanIn()[1]->_value   );
                }
                else if (fanout->getModel() == "NOR2") {
                    fanout->_value = !(fanout->getFanIn()[0]->_value ||
                                      fanout->getFanIn()[1]->_value   );
                }
                else if (fanout->getModel() == "NOT1") {
                    fanout->_value = !fanout->getFanIn()[0]->_value;
                }
                else if (fanout->getModel() == "PO") {
                    fanout->_value = fanout->getFanIn()[0]->_value;
                }

                // Assign arrival time to that fan-out.
                if (fanout->getModel() == "PO") {
                    fanout->_arrivalTime = gate->_arrivalTime;

                    // No need to put PO into queue, because PO does
                    // not have fan-out.
                }
                else {
                    fanout->_arrivalTime = gate->_arrivalTime + 1;

                    // Add that fan-out to queue.
                    q.push(fanout);
                }
            }
        }
    }
}

#define ASSERT(condition)                                                \
    if (!(condition)) {                                                  \
        std::cerr << "Error: Gate '" << g1->getName() << "' in Path { "  \
                  << path_idx + 1 << " } is not part of true path."      \
                  << "\nnext->getName()  = " << next->getName()          \
                  << "\nnext->getModel() = " << next->getModel()         \
                  << "\nnext->_value     = " << (int)(next->_value)      \
                  << "\ng1->getName()    = " << g1->getName()            \
                  << "\ng1->_value       = " << (int)(g1->_value)        \
                  << "\ng2->getName()    = " << g2->getName()            \
                  << "\ng2->_value       = " << (int)(g2->_value)        \
                  << "\n";                                               \
        return 1;                                                        \
    }

// Verify a whether a path is a true path. Return 0 if it is true.
static int verify_true_path(Sta::Cir::Circuit&        cir,
                            const Sta::Cir::Path&     path,
                            size_t                    path_idx,
                            const std::vector<bool>&  path_value,
                            const Sta::Cir::InputVec& input_vec  ) {
    using Sta::Cir::Gate;
    
    typedef std::map<std::string, Gate*>::iterator Iter;

    int i = 0;
    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it, ++i                          ) {

        it->second->_value = input_vec[i];
    }
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        cir.getOutputs()[i]->_value = 2;
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_value = 2;
    }

    calculate_value(cir);

    for (size_t i = 0; i < path.size() - 1; ++i) {
        Gate* g1   = path[i];
        Gate* next = path[i + 1];
        Gate* g2   = 0;

        if (next->getModel() == "NAND2" ||
            next->getModel() == "NOR2"    ) {

            // Try to find side input
            if (next->getFanIn()[0] == g1) {
                g2 = next->getFanIn()[1];
            }
            else {
                g2 = next->getFanIn()[0];
            }

            if (next->getModel() == "NAND2") {
                ASSERT(g1->_value == 0 || g2->_value == 1)
            }
            else { // next->getModel() == "NOR2"
                ASSERT(g1->_value == 1 || g2->_value == 0)
            }
        }


        if (g1->_value != path_value[i]) {
            std::cerr << "Error: Gate '" << g1->getName() << "' _value "
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
    std::cerr << "Number of verified path:  ";
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
