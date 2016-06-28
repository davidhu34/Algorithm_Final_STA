#include "sta/src/cir/circuit.h"

#include <queue>

#include "sta/src/util/hash_map.h"
#include "sta/src/util/prime.h"
#include "sta/src/util/hasher.h"

#define CREATE_NEW_GATE(group)                                       \
    for (size_t i = 0; i < cir.group.size(); ++i) {                  \
        const Gate* g = cir.group[i];                                \
        Gate* new_g   = new Gate(*g);                                \
        group[i]      = new_g;                                       \
        new_gate.bucket[0].push_back(HMap::Pair(g, new_g));          \
        new_gate.size += 1;                                          \
    }

#define LINK_ALL_GATES(group)                                 \
    for (size_t i = 0; i < cir.group.size(); ++i) {           \
        const Gate* gate = cir.group[i];                      \
                                                              \
        for (size_t j = 0; j < gate->tos.size(); ++j) {       \
            const Gate* fanout = gate->tos[j];                \
            new_gate[gate]->tos[j] = new_gate[fanout];        \
                                                              \
            if (fanout->froms[0] == gate) {                   \
                new_gate[fanout]->froms[0] = new_gate[gate];  \
            }                                                 \
            else { /* fanout->froms[1] == gate */             \
                new_gate[fanout]->froms[1] = new_gate[gate];  \
            }                                                 \
        }                                                     \
    }

Sta::Cir::Circuit::Circuit(const Circuit& cir):
    name           (cir.name                  ),
    primary_inputs (cir.primary_inputs.size() ),
    primary_outputs(cir.primary_outputs.size()),
    logic_gates    (cir.logic_gates.size()    ),
    modules        (cir.modules               )  {

    using Util::PtrHashFunc;
    typedef Util::HashMap<const Gate*, Gate*, PtrHashFunc> HMap;
    
    HMap new_gate(Util::hash_ptr);

    CREATE_NEW_GATE(primary_outputs)
    CREATE_NEW_GATE(primary_inputs)
    CREATE_NEW_GATE(logic_gates)

    new_gate.rehash(Util::prime_gt(new_gate.size));

    LINK_ALL_GATES(primary_inputs)
    LINK_ALL_GATES(logic_gates)
}

#define CLEAR_GATE(group)                       \
    for (size_t i = 0; i < group.size(); ++i) { \
        delete group[i];                        \
    }

Sta::Cir::Circuit::~Circuit() {
    CLEAR_GATE(primary_inputs)
    CLEAR_GATE(primary_outputs)
    CLEAR_GATE(logic_gates)
}
