#include "sta/src/ana/analyzer.h"

#include <assert.h>
#include <queue>

#include "sta/src/util/backup.h"

static bool in_queue(Sta::Cir::GateCopy* gate) {
    return gate->tag & 1;
}

static void in_queue(Sta::Cir::GateCopy* gate, bool value) {
    if (value) {
        gate->tag |= 1;
    }
    else {
        gate->tag &= ~1;
    }
}

static bool is_true_path(Sta::Cir::GateCopy* gate) {
    return gate->tag & 2;
}

static void is_true_path(Sta::Cir::GateCopy* gate, bool value) {
    if (value) {
        gate->tag |= 2;
    }
    else {
        gate->tag &= ~2;
    }
}

#define RESET_GATE_OF_GROUP(group)                    \
    for (size_t i = 0; i < cir.group.size(); ++i) {   \
        GateCopy* g = cir.group[i];                   \
        g->tag = 0;                                   \
        g->arrival_time = UNKNOWN;                    \
    }

static void reset_gate(const Sta::Cir::Subcircuit& cir) {
    using Sta::Cir::GateCopy;
    using Sta::Cir::Time::UNKNOWN;

    RESET_GATE_OF_GROUP(primary_inputs)
    RESET_GATE_OF_GROUP(primary_outputs)
    RESET_GATE_OF_GROUP(logic_gates)
}

static void assign_gate(const Sta::Cir::PathCopy& true_path) {
    for (size_t i = 0; i < true_path.size(); ++i) {
        is_true_path(true_path[i], true);
        true_path[i]->arrival_time = true_path.size() - i - 1;
    }

    true_path[0]->arrival_time = true_path[1]->arrival_time;
}

static void push_fanout_to_queue(
    Sta::Cir::GateCopy*              gate,
    std::queue<Sta::Cir::GateCopy*>& q    ) {

    using Sta::Cir::GateCopy;

    for (size_t i = 0; i < gate->tos.size(); ++i) {
        GateCopy* g = gate->tos[i];
        if (!in_queue(g)) { // not in queue
            in_queue(g, true);
            q.push(g);
        }
    }
}

static void update_value(
    Sta::Cir::GateCopy*                    gate,
    uint8_t                                value,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    backup.sync_if_no_copy(gate);
    gate->value = value;

    if (!in_queue(gate)) { // not in queue
        in_queue(gate, true);
        q.push(gate);
    }

    push_fanout_to_queue(gate, q);
}

static void update_timing(
    Sta::Cir::GateCopy*                    gate,
    Sta::Cir::GateCopy*                    pin,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    backup.sync_if_no_copy(gate);
    gate->arrival_time = pin->arrival_time + 1;

    push_fanout_to_queue(gate, q);
}

static bool update_nand_gate_value(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;

    GateCopy* gA = gate->froms[0];
    GateCopy* gB = gate->froms[1];

    if (gate->value == 0) {
        if (gA->value == 0 || gB->value == 0) {
            return false;
        }
        if (gA->value == 2) {
            update_value(gA, 1, q, backup);
        }
        if (gB->value == 2) {
            update_value(gB, 1, q, backup);
        }
    }
    else if (gate->value == 1) {
        if (gA->value == 1 && gB->value == 1) {
            return false;
        }
        if (gA->value == 1 && gB->value == 2) {
            update_value(gB, 0, q, backup);
        }
        else if (gA->value == 2 && gB->value == 1) {
            update_value(gA, 0, q, backup);
        }
    }
    else {
        assert(gate->value == 2);
        if (gA->value == 0 || gB->value == 0) {
            update_value(gate, 1, q, backup);
        }
        else if (gA->value == 1 && gB->value == 1) {
            update_value(gate, 0, q, backup);
        }
    }

    return true;
}

static bool update_nor_gate_value(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;

    GateCopy* gA = gate->froms[0];
    GateCopy* gB = gate->froms[1];

    if (gate->value == 1) {
        if (gA->value == 1 || gB->value == 1) {
            return false;
        }
        if (gA->value == 2) {
            update_value(gA, 0, q, backup);
        }
        if (gB->value == 2) {
            update_value(gB, 0, q, backup);
        }
    }
    else if (gate->value == 0) {
        if (gA->value == 0 && gB->value == 0) {
            return false;
        }
        if (gA->value == 1 && gB->value == 2) {
            update_value(gB, 1, q, backup);
        }
        else if (gA->value == 2 && gB->value == 1) {
            update_value(gA, 1, q, backup);
        }
    }
    else {
        assert(gate->value == 2);
        if (gA->value == 1 || gB->value == 1) {
            update_value(gate, 0, q, backup);
        }
        else if (gA->value == 0 && gB->value == 0) {
            update_value(gate, 1, q, backup);
        }
    }

    return true;
}

static bool update_not_gate_value(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;

    GateCopy* gA = gate->froms[0];

    if (gate->value == 0) {
        if (gA->value == 0) {
            return false;
        }
        if (gA->value == 2) {
            update_value(gA, 1, q, backup);
        }
    }
    else if (gate->value == 1) {
        if (gA->value == 1) {
            return false;
        }
        if (gA->value == 2) {
            update_value(gA, 0, q, backup);
        }
    }
    else {
        assert(gate->value == 2);
        if (gA->value == 0) {
            update_value(gate, 1, q, backup);
        }
        else if (gA->value == 1) {
            update_value(gate, 0, q, backup);
        }
    }

    return true;
}

static bool update_nand_gate_timing(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;
    using Sta::Cir::Time::UNKNOWN;

    GateCopy* gA = gate->froms[0];
    GateCopy* gB = gate->froms[1];

    if (is_true_path(gA)) {
        if (gB->arrival_time == UNKNOWN) {
            if (gA->value == 1 && gB->value == 1) {
                if (gB->orig->min_arrival_time > gA->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 0 && gB->value == 0) {
                if (gB->orig->max_arrival_time < gA->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 0 && gB->value == 2) {
                if (gB->orig->max_arrival_time < gA->arrival_time) {
                    update_value(gB, 1, q, backup);
                }
            }
        }
        else { // gB->arrival_time != UNKNOWN
            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 1 and gB->value == 1) {
                    return false;
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gA->value == 0 and gB->value == 0) {
                    return false;
                }
            }
        }
    }
    else if (is_true_path(gB)) {
        if (gA->arrival_time == UNKNOWN) {
            if (gA->value == 1 && gB->value == 1) {
                if (gA->orig->min_arrival_time > gB->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 0 && gB->value == 0) {
                if (gA->orig->max_arrival_time < gB->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 2 && gB->value == 0) {
                if (gA->orig->max_arrival_time < gB->arrival_time) {
                    update_value(gA, 1, q, backup);
                }
            }
        }
        else { // gA->arrival_time != UNKNOWN
            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 0 and gB->value == 0) {
                    return false;
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gA->value == 1 and gB->value == 1) {
                    return false;
                }
            }
        }
    }
    else {
        assert(!is_true_path(gA) && !is_true_path(gB));
        if (gA->arrival_time != UNKNOWN && 
            gB->arrival_time != UNKNOWN   ) {

            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 0) {
                    update_timing(gate, gA, q, backup);
                }
                else if (gA->value == 1) {
                    update_timing(gate, gB, q, backup);
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gB->value == 0) {
                    update_timing(gate, gB, q, backup);
                }
                else if (gB->value == 1) {
                    update_timing(gate, gA, q, backup);
                }
            }
            else { // gA->arrival_time == gB->arrival_time
                update_timing(gate, gA, q, backup);
            }
        }
    }

    return true;
}

static bool update_nor_gate_timing(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;
    using Sta::Cir::Time::UNKNOWN;

    GateCopy* gA = gate->froms[0];
    GateCopy* gB = gate->froms[1];

    if (is_true_path(gA)) {
        if (gB->arrival_time == UNKNOWN) {
            if (gA->value == 0 && gB->value == 0) {
                if (gB->orig->min_arrival_time > gA->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 1 && gB->value == 1) {
                if (gB->orig->max_arrival_time < gA->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 1 && gB->value == 2) {
                if (gB->orig->max_arrival_time < gA->arrival_time) {
                    update_value(gB, 0, q, backup);
                }
            }
        }
        else { // gB->arrival_time != UNKNOWN
            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 0 and gB->value == 0) {
                    return false;
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gA->value == 1 and gB->value == 1) {
                    return false;
                }
            }
        }
    }
    else if (is_true_path(gB)) {
        if (gA->arrival_time == UNKNOWN) {
            if (gA->value == 0 && gB->value == 0) {
                if (gA->orig->min_arrival_time > gB->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 1 && gB->value == 1) {
                if (gA->orig->max_arrival_time < gB->arrival_time) {
                    return false;
                }
            }
            else if (gA->value == 2 && gB->value == 1) {
                if (gA->orig->max_arrival_time < gB->arrival_time) {
                    update_value(gA, 0, q, backup);
                }
            }
        }
        else { // gA->arrival_time != UNKNOWN
            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 1 and gB->value == 1) {
                    return false;
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gA->value == 0 and gB->value == 0) {
                    return false;
                }
            }
        }
    }
    else {
        assert(!is_true_path(gA) && !is_true_path(gB));
        if (gA->arrival_time != UNKNOWN && 
            gB->arrival_time != UNKNOWN   ) {

            if (gA->arrival_time < gB->arrival_time) {
                if (gA->value == 1) {
                    update_timing(gate, gA, q, backup);
                }
                else if (gA->value == 0) {
                    update_timing(gate, gB, q, backup);
                }
            }
            else if (gA->arrival_time > gB->arrival_time) {
                if (gB->value == 1) {
                    update_timing(gate, gB, q, backup);
                }
                else if (gB->value == 0) {
                    update_timing(gate, gA, q, backup);
                }
            }
            else { // gA->arrival_time == gB->arrival_time
                update_timing(gate, gA, q, backup);
            }
        }
    }

    return true;
}

static bool update_not_gate_timing(
    Sta::Cir::GateCopy*                    gate,
    std::queue<Sta::Cir::GateCopy*>&       q,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::Time::UNKNOWN;

    if (gate->froms[0]->arrival_time != UNKNOWN &&
        gate->arrival_time == UNKNOWN             ) {
        update_timing(gate, gate->froms[0], q, backup);
    }

    assert(!(gate->froms[0]->arrival_time == UNKNOWN &&
             gate->arrival_time != UNKNOWN             ));

    return true;
}

static bool evaluate(
    const Sta::Cir::PathCopy&              updated_gate_list,
    Sta::Util::Backup<Sta::Cir::GateCopy>& backup) {

    using Sta::Cir::GateCopy;
    using Sta::Cir::Module;
    using Sta::Cir::Time::UNKNOWN;

    std::queue<GateCopy*> q;

    for (size_t i = 0; i < updated_gate_list.size(); ++i) {
        GateCopy* gate = updated_gate_list[i];

        q.push(gate);
        in_queue(gate, true);
        push_fanout_to_queue(gate, q);
    }

    while (!q.empty()) {
        GateCopy* gate = q.front();
        q.pop();

        switch (gate->orig->module) {
        case Module::NAND2:
            if (!update_nand_gate_value(gate, q, backup) ||
                !update_nand_gate_timing(gate, q, backup)  ) {
                return false;
            }
            break;

        case Module::NOR2:
            if (!update_nor_gate_value(gate, q, backup) ||
                !update_nor_gate_timing(gate, q, backup)  ) {
                return false;
            }
            break;
            
        case Module::NOT1:
            if (!update_not_gate_value(gate, q, backup) ||
                !update_not_gate_timing(gate, q, backup)  ) {
                return false;
            }
            break;

        case Module::PI:
            assert(gate->arrival_time != UNKNOWN);
            break;

        case Module::PO:
            if (gate->arrival_time == UNKNOWN || gate->value == 2) {
                backup.sync_if_no_copy(gate);
                gate->arrival_time = gate->froms[0]->arrival_time;
                gate->value = gate->froms[0]->value;
            }
            break;

        default:
            assert(false && "Unknown gate type.");
        }

        in_queue(gate, false); // not in queue
    }

    return true;
}

static void get_input_vec(const Sta::Cir::Subcircuit& cir, 
                          Sta::Cir::InputVec&         input_vec) {

    assert(input_vec.size() == cir.orig.primary_inputs.size());

    for (size_t i = 0, j = 0; i < cir.primary_inputs.size(); ++i, ++j) {
        while (cir.orig.primary_inputs[j] != 
               cir.primary_inputs[i]->orig   ) {
            input_vec[j] = 0;
            ++j;
        }

        assert(cir.primary_inputs[i]->value == 1 ||
               cir.primary_inputs[i]->value == 0   );

        input_vec[j] = cir.primary_inputs[i]->value;
    }
}

static size_t next_PI_that_has_unknown_value(
    const Sta::Cir::Subcircuit& cir,
    size_t                      beg) {

    for (size_t i = beg; i < cir.primary_inputs.size(); ++i) {
        if (cir.primary_inputs[i]->value == 2) {
            return i;
        }
    }

    return cir.primary_inputs.size();
}

static bool guess(const Sta::Cir::Subcircuit& cir,
                  Sta::Cir::GateCopy*         gate,
                  uint8_t                     value,
                  Sta::Cir::InputVec&         input_vec) {

    using Sta::Cir::GateCopy;
    using Sta::Cir::PathCopy;
    using Sta::Util::Backup;

    Backup<GateCopy> backup;
    backup.sync_if_no_copy(gate);
    gate->value = value;

    if (!evaluate(PathCopy(1, gate), backup)) {
        return false;
    }

    size_t idx = next_PI_that_has_unknown_value(cir, 0);
    if (idx == cir.primary_inputs.size()) { // all PI has known value
        get_input_vec(cir, input_vec);
        return true;
    }

    if (guess(cir, cir.primary_inputs[idx], 0, input_vec)) {
        return true;
    }
    if (guess(cir, cir.primary_inputs[idx], 1, input_vec)) {
        return true;
    }

    return false;
}

bool Sta::Ana::no_conflict_2(const Cir::Subcircuit& cir,
                             const Cir::PathCopy&   path,
                             Cir::PathCopy          subpath,
                             Cir::InputVec&         input_vec) {

    reset_gate(cir);   // in_queue, is_true_path, arrival_time
    assign_gate(path); // is_true_path, arrival_time

    Util::Backup<Cir::GateCopy> backup;

    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Cir::GateCopy* gate = cir.primary_inputs[i];

        backup.sync_if_no_copy(gate);
        gate->arrival_time = 0;
        subpath.push_back(gate);
    }

    if (!evaluate(subpath, backup)) {
        return false;
    }

    size_t idx = next_PI_that_has_unknown_value(cir, 0);
    if (idx == cir.primary_inputs.size()) { // all PI has known value
        get_input_vec(cir, input_vec);
        return true;
    }

    if (guess(cir, cir.primary_inputs[idx], 0, input_vec)) {
        return true;
    }
    if (guess(cir, cir.primary_inputs[idx], 1, input_vec)) {
        return true;
    }

    return false;
}
