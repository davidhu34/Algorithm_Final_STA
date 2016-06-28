#include "sta/src/cir/subcircuit.h"

#include <queue>

#include "sta/src/util/hash_map.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/prime.h"

typedef Sta::Util::HashMap<
    const Sta::Cir::Gate*, 
    Sta::Cir::GateCopy*, 
    Sta::Util::PtrHashFunc> GateMap;

static void create_connect_add_to_queue(
    const Sta::Cir::Gate*              gate,
    const Sta::Cir::Gate*              fanin,
    GateMap&                           ng,
    std::queue<const Sta::Cir::Gate*>& q) {

    assert(gate != fanin);

    using Sta::Cir::GateCopy;

    GateCopy* gate_cp = ng[gate];
    GateCopy* fanin_cp;

    if (ng.contains(fanin)) {
        fanin_cp = ng[fanin];
    }
    else {
        fanin_cp = new GateCopy(fanin);
        ng.insert_blindly(fanin, fanin_cp);
        q.push(fanin);
    }

    gate_cp->froms.push_back(fanin_cp);
    fanin_cp->tos.push_back(gate_cp);
}

static void add_gates(GateMap& ng, Sta::Cir::Subcircuit* cir) {
    using Sta::Cir::Gate;
    using Sta::Cir::Module;

    cir->primary_inputs.reserve(cir->orig.primary_inputs.size());
    cir->primary_outputs.reserve(1);
    cir->logic_gates.reserve(ng.size);

    for (size_t i = 0; i < ng.bucket.size(); ++i) {
        for (size_t j = 0; j < ng.bucket[i].size(); ++j) {
            switch (ng.bucket[i][j].key->module) {
            case Module::NAND2:
            case Module::NOR2:
            case Module::NOT1:
                cir->logic_gates.push_back(ng.bucket[i][j].value);
                break;

            case Module::PO:
                cir->primary_outputs.push_back(ng.bucket[i][j].value);
                break;
            }
        }
    }

    for (size_t i = 0; i < cir->orig.primary_inputs.size(); ++i) {
        Gate* pi = cir->orig.primary_inputs[i];

        if (ng.contains(pi)) {
            cir->primary_inputs.push_back(ng[pi]);
        }
    }
}

Sta::Cir::Subcircuit::Subcircuit(const Circuit& cir, const Gate* g):
    orig(cir) {

    GateMap ng(Util::hash_ptr); // new gates
    ng.bucket.resize(Util::prime_gt(
        orig.primary_inputs.size() +
        orig.primary_outputs.size() +
        orig.logic_gates.size()));

    std::queue<const Gate*> q;
    q.push(g);
    ng.insert_blindly(g, new GateCopy(g));

    while (!q.empty()) {
        g = q.front();
        q.pop();
        
        switch (g->module) {
        case Module::NAND2:
        case Module::NOR2:
            create_connect_add_to_queue(g, g->froms[0], ng, q);
            create_connect_add_to_queue(g, g->froms[1], ng, q);
            break;

        case Module::NOT1:
        case Module::PO:
            create_connect_add_to_queue(g, g->froms[0], ng, q);
            break;
        }
    }

    add_gates(ng, this);
}

#define CLEAR_GATE(group)                       \
    for (size_t i = 0; i < group.size(); ++i) { \
        delete group[i];                        \
    }

Sta::Cir::Subcircuit::~Subcircuit() {
    CLEAR_GATE(primary_inputs)
    CLEAR_GATE(primary_outputs)
    CLEAR_GATE(logic_gates)
}

