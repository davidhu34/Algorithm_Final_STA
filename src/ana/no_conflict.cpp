#include "sta/src/ana/analyzer.h"

#include <assert.h>
#include <queue>

// Pack value of gates into bit flags.
static uint8_t pack_value(const Sta::Cir::Gate* gY, 
                          const Sta::Cir::Gate* gA,
                          const Sta::Cir::Gate* gB ) {
    //    +-------Value of Y
    //    |  +----Value of A
    //    |  |  +-Value of B
    //    |  |  |
    // XX XX XX XX
    //
    uint8_t bits = gY->value;
    bits <<= 2;
    bits |= gA->value;
    bits <<= 2;
    bits |= gB->value;

    return bits;
}

static uint8_t pack_value(const Sta::Cir::Gate* gY, 
                          const Sta::Cir::Gate* gA ) {
    //       +----Value of Y
    //       |  +-Value of A
    //       |  |
    // XX XX XX XX
    //
    uint8_t bits = gY->value;
    bits <<= 2;
    bits |= gA->value;

    return bits;
}

// Pack arrival time of gates into bit flags.
static uint8_t pack_atime(const Sta::Cir::Gate* gY, 
                          const Sta::Cir::Gate* gA,
                          const Sta::Cir::Gate* gB ) {

    using Sta::Cir::Time::UNKNOWN;

    //   +-----------T
    //   |  +--------Yt
    //   |  | +------At
    //   |  | | +----Bt
    //   |  | | | +--Q
    //   |  | | | |
    // X XX X X X XX
    //
    // T: Trying to make __ true path
    // - 00 = A
    // - 01 = B
    // - 10 = No
    //
    // Yt, At, Bt: Known arrival time?
    // - 0 = unknown
    // - 1 = known
    //
    // Q: Compare arrival time
    // - 00 = unknown
    // - 01 = Att <  Btt
    // - 10 = Att >  Btt
    // - 11 = Att == Btt
    //
    // Att: Arrival time of A
    //
    uint8_t bits = 0;

    if (gY->is_true_path) {
        assert(gA->is_true_path ^ gB->is_true_path);

        if (gA->is_true_path) {
            bits |= 0;
        }
        else {
            bits |= 1;
        }
    }
    else {
        bits |= 2;
    }

    bits <<= 1;
    if (gY->arrival_time != UNKNOWN) {
        bits |= 1;
    }

    bits <<= 1;
    if (gA->arrival_time != UNKNOWN) {
        bits |= 1;
    }

    bits <<= 1;
    if (gB->arrival_time != UNKNOWN) {
        bits |= 1;
    }

    bits <<= 2;
    if (gA->arrival_time != UNKNOWN && gB->arrival_time != UNKNOWN) {
        if (gA->arrival_time < gB->arrival_time) {
            bits |= 1;
        }
        else if (gA->arrival_time > gB->arrival_time) {
            bits |= 2;
        }
        else {
            bits |= 3;
        }
    }

    return bits;
}

static uint8_t pack_atime(const Sta::Cir::Gate* gY, 
                          const Sta::Cir::Gate* gA ) {

    using Sta::Cir::Time::UNKNOWN;

    //       +-------T
    //       |  +----Yt
    //       |  | +--At
    //       |  | |
    // XX XX XX X X
    //
    // T: Trying to make __ true path
    // - 00 = A
    // - 10 = No
    //
    // Yt, At: Known arrival time?
    // - 0 = unknown
    // - 1 = known
    //
    uint8_t bits = 0;
    assert(gA->is_true_path == gY->is_true_path);

    if (gY->is_true_path) {
        bits |= 0;
    }
    else {
        bits |= 2;
    }

    bits <<= 1;
    if (gY->arrival_time != UNKNOWN) {
        bits |= 1;
    }

    bits <<= 1;
    if (gA->arrival_time != UNKNOWN) {
        bits |= 1;
    }

    return bits;
}

#define PUSH_FANOUT_TO_QUEUE                                         \
    for (size_t i = 0; i < gate->tos.size(); ++i) {                  \
        if (gate->tos[i]->tag == 0) {                                \
            gate->tos[i]->tag = 1;                                   \
            q.push(gate->tos[i]);                                    \
        }                                                            \
    }

#define ASSIGN_Y(val)                                                \
    gate->value = val;                                               \
    value_state &= ~0x30;                                            \
    value_state |= (val << 4);                                       \
    PUSH_FANOUT_TO_QUEUE

#define ASSIGN_PIN(pin, val)                                         \
    pin->value = val;                                                \
    if (pin->tag == 0) {                                             \
        pin->tag = 1;                                                \
        q.push(pin);                                                 \
    }

#define ASSIGN_A(val)                                                \
    value_state &= ~0xc;                                             \
    value_state |= (val << 2);                                       \
    ASSIGN_PIN(gA, val)

#define ASSIGN_B(val)                                                \
    value_state &= ~0x3;                                             \
    value_state |= val;                                              \
    ASSIGN_PIN(gB, val)

#define ASSIGN_YTT_FROM(pin)                                         \
    gate->arrival_time = pin->arrival_time + 1;                      \
    atime_state |= (1 << 4);                                         \
    PUSH_FANOUT_TO_QUEUE

#define IF_BMINT_GT_ATT_THEN_RETURN_F                                \
    if (gB->min_arrival_time > gA->arrival_time) {                   \
        return false;                                                \
    }

#define IF_BMAXT_LT_ATT_THEN_RETURN_F                                \
    if (gB->max_arrival_time < gA->arrival_time) {                   \
        return false;                                                \
    }

#define IF_BMAXT_LT_ATT_THEN_B_IS(val)                               \
    if (gB->max_arrival_time < gA->arrival_time) {                   \
        ASSIGN_B(val)                                                \
    }

#define IF_AMINT_GT_BTT_THEN_RETURN_F                                \
    if (gA->min_arrival_time > gB->arrival_time) {                   \
        return false;                                                \
    }

#define IF_AMAXT_LT_BTT_THEN_RETURN_F                                \
    if (gA->max_arrival_time < gB->arrival_time) {                   \
        return false;                                                \
    }

#define IF_AMAXT_LT_BTT_THEN_A_IS(val)                               \
    if (gA->max_arrival_time < gB->arrival_time) {                   \
        ASSIGN_A(val)                                                \
    }

#define CASE_NAND2_01100                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ IF_BMINT_GT_ATT_THEN_RETURN_F      break; \
    case 0x10: /* 1 0 0 */ IF_BMAXT_LT_ATT_THEN_RETURN_F      break; \
    case 0x11: /* 1 0 1 */                                    break; \
    case 0x12: /* 1 0 2 */ IF_BMAXT_LT_ATT_THEN_B_IS(1)       break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_01111                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ return false;                      break; \
    case 0x10: /* 1 0 0 */                                    break; \
    case 0x11: /* 1 0 1 */                                    break; \
    case 0x12: /* 1 0 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_01112                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x10: /* 1 0 0 */ return false;                      break; \
    case 0x11: /* 1 0 1 */                                    break; \
    case 0x12: /* 1 0 2 */ ASSIGN_B(1)                        break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_01113                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x10: /* 1 0 0 */                                    break; \
    case 0x11: /* 1 0 1 */                                    break; \
    case 0x12: /* 1 0 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_11010                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ IF_AMINT_GT_BTT_THEN_RETURN_F      break; \
    case 0x10: /* 1 0 0 */ IF_AMAXT_LT_BTT_THEN_RETURN_F      break; \
    case 0x14: /* 1 1 0 */                                    break; \
    case 0x18: /* 1 2 0 */ IF_AMAXT_LT_BTT_THEN_A_IS(1)       break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_11111                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x10: /* 1 0 0 */ return false;                      break; \
    case 0x14: /* 1 1 0 */                                    break; \
    case 0x18: /* 1 2 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_11112                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ return false;                      break; \
    case 0x10: /* 1 0 0 */                                    break; \
    case 0x14: /* 1 1 0 */                                    break; \
    case 0x18: /* 1 2 0 */ ASSIGN_A(1)                        break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_11113                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x10: /* 1 0 0 */                                    break; \
    case 0x14: /* 1 1 0 */                                    break; \
    case 0x18: /* 1 2 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_20111                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x10: /* 1 0 0 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x11: /* 1 0 1 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x12: /* 1 0 2 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x14: /* 1 1 0 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x18: /* 1 2 0 */                                    break; \
    case 0x1a: /* 1 2 2 */                                    break; \
    case 0x26: /* 2 1 2 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x29: /* 2 2 1 */                                    break; \
    case 0x2a: /* 2 2 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NAND2_20112                                             \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x05: /* 0 1 1 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x10: /* 1 0 0 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x11: /* 1 0 1 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x12: /* 1 0 2 */                                    break; \
    case 0x14: /* 1 1 0 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x18: /* 1 2 0 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x1a: /* 1 2 2 */                                    break; \
    case 0x26: /* 2 1 2 */                                    break; \
    case 0x29: /* 2 2 1 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x2a: /* 2 2 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_01100                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x04: /* 0 1 0 */                                    break; \
    case 0x05: /* 0 1 1 */ IF_BMAXT_LT_ATT_THEN_RETURN_F      break; \
    case 0x06: /* 0 1 2 */ IF_BMAXT_LT_ATT_THEN_B_IS(0)       break; \
    case 0x10: /* 1 0 0 */ IF_BMINT_GT_ATT_THEN_RETURN_F      break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_01111                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x04: /* 0 1 0 */                                    break; \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x06: /* 0 1 2 */                                    break; \
    case 0x10: /* 1 0 0 */ return false;                      break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_01112                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x04: /* 0 1 0 */                                    break; \
    case 0x05: /* 0 1 1 */ return false;                      break; \
    case 0x06: /* 0 1 2 */ ASSIGN_B(0)                        break; \
    case 0x10: /* 1 0 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_01113                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x04: /* 0 1 0 */                                    break; \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x06: /* 0 1 2 */                                    break; \
    case 0x10: /* 1 0 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_11010                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */                                    break; \
    case 0x05: /* 0 1 1 */ IF_AMAXT_LT_BTT_THEN_RETURN_F      break; \
    case 0x09: /* 0 2 1 */ IF_AMAXT_LT_BTT_THEN_A_IS(0)       break; \
    case 0x10: /* 1 0 0 */ IF_AMINT_GT_BTT_THEN_RETURN_F      break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_11111                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */                                    break; \
    case 0x05: /* 0 1 1 */ return false;                      break; \
    case 0x09: /* 0 2 1 */ ASSIGN_A(0)                        break; \
    case 0x10: /* 1 0 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_11112                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */                                    break; \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x09: /* 0 2 1 */                                    break; \
    case 0x10: /* 1 0 0 */ return false;                      break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_11113                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */                                    break; \
    case 0x05: /* 0 1 1 */                                    break; \
    case 0x09: /* 0 2 1 */                                    break; \
    case 0x10: /* 1 0 0 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_20111                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x04: /* 0 1 0 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x05: /* 0 1 1 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x06: /* 0 1 2 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x09: /* 0 2 1 */                                    break; \
    case 0x0a: /* 0 2 2 */                                    break; \
    case 0x10: /* 1 0 0 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x22: /* 2 0 2 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x28: /* 2 2 0 */                                    break; \
    case 0x2a: /* 2 2 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

#define CASE_NOR2_20112                                              \
    switch (value_state) {                                           \
               /* Y A B */                                           \
    case 0x01: /* 0 0 1 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x04: /* 0 1 0 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x05: /* 0 1 1 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x06: /* 0 1 2 */                                    break; \
    case 0x09: /* 0 2 1 */ ASSIGN_YTT_FROM(gB)                break; \
    case 0x0a: /* 0 2 2 */                                    break; \
    case 0x10: /* 1 0 0 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x22: /* 2 0 2 */                                    break; \
    case 0x28: /* 2 2 0 */ ASSIGN_YTT_FROM(gA)                break; \
    case 0x2a: /* 2 2 2 */                                    break; \
    default: assert(0 && "Unknown value state.");             break; \
    }

bool Sta::Ana::no_conflict(const Cir::Circuit&   cir,
                           const Cir::Path&      path,
                           const Cir::PathValue& path_value,
                           const Cir::Path&      subpath,
                           const Cir::PathValue& subpath_value) {

    assert(path.size() == path_value.size());
    assert(subpath.size() == subpath_value.size());

    using Sta::Cir::Gate;
    using Sta::Cir::Module;
    using Sta::Cir::Time::UNKNOWN;
    
    // Initialize tag, value, is_true_path and arrival time.
#define RESET_GATE_OF_GROUP(group)                                   \
    for (size_t i = 0; i < cir.group.size(); ++i) {                  \
        Gate* g = cir.group[i];                                      \
        g->tag          = 0;                                         \
        g->value        = 2;                                         \
        g->is_true_path = false;                                     \
        g->arrival_time = UNKNOWN;                                   \
    }

    RESET_GATE_OF_GROUP(primary_inputs)
    RESET_GATE_OF_GROUP(primary_outputs)
    RESET_GATE_OF_GROUP(logic_gates)

    for (size_t i = 0; i < path.size(); ++i) {
        // Assign value along path.
        path[i]->value = path_value[i];

        // Note down which gate is true path.
        path[i]->is_true_path = true;

        // Assign arrival time along path.
        path[i]->arrival_time = path.size() - i - 1;
    }
    // Arrival time of PO is same as its fan-in.
    path[0]->arrival_time = path[1]->arrival_time;

    // Create and initialize queue. Every gate in queue need to check
    // its state for update. If there is any update, add corresponding
    // gate into queue. E.g. If arrival time is updated, add all 
    // fan-out into queue; If value of B is updated, add fan-in that
    // connect to pin B into queue.
    //
    std::queue<Gate*> q;

    for (size_t i = 0; i < subpath.size(); ++i) {
        Gate* g = subpath[i];

        // Assign value.
        g->value = subpath_value[i];

        // Push it into queue.
        if (g->tag == 0) {
            g->tag = 1;
            q.push(g);
        }
    }

    // Push PI into queue.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* g = cir.primary_inputs[i];
        if (g->tag == 0) {
            g->tag = 1;
            q.push(g);
        }
    }

    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        gate->tag = 0;

        switch (gate->module) {
        case Module::NAND2: {
            Gate* gA = gate->froms[0];
            Gate* gB = gate->froms[1];

            uint8_t value_state = pack_value(gate, gA, gB);
            
            switch (value_state) {
                       /* Y A B */
            case 0x00: /* 0 0 0 */ return false;              break;
            case 0x01: /* 0 0 1 */ return false;              break;
            case 0x02: /* 0 0 2 */ return false;              break;
            case 0x04: /* 0 1 0 */ return false;              break;
            case 0x05: /* 0 1 1 */                            break;
            case 0x06: /* 0 1 2 */ ASSIGN_B(1)                break;
            case 0x08: /* 0 2 0 */ return false;              break;
            case 0x09: /* 0 2 1 */ ASSIGN_A(1)                break;
            case 0x0a: /* 0 2 2 */ ASSIGN_A(1) ASSIGN_B(1)    break;
                
            case 0x10: /* 1 0 0 */                            break;
            case 0x11: /* 1 0 1 */                            break;
            case 0x12: /* 1 0 2 */                            break;
            case 0x14: /* 1 1 0 */                            break;
            case 0x15: /* 1 1 1 */ return false;              break;
            case 0x16: /* 1 1 2 */ ASSIGN_B(0)                break;
            case 0x18: /* 1 2 0 */                            break;
            case 0x19: /* 1 2 1 */ ASSIGN_A(0)                break;
            case 0x1a: /* 1 2 2 */                            break;

            case 0x20: /* 2 0 0 */ ASSIGN_Y(1)                break;
            case 0x21: /* 2 0 1 */ ASSIGN_Y(1)                break;
            case 0x22: /* 2 0 2 */ ASSIGN_Y(1)                break;
            case 0x24: /* 2 1 0 */ ASSIGN_Y(1)                break;
            case 0x25: /* 2 1 1 */ ASSIGN_Y(0)                break;
            case 0x26: /* 2 1 2 */                            break;
            case 0x28: /* 2 2 0 */ ASSIGN_Y(1)                break;
            case 0x29: /* 2 2 1 */                            break;
            case 0x2a: /* 2 2 2 */                            break;
            default: assert(0 && "Unknown value state.");     break;
            }

            uint8_t atime_state = pack_atime(gate, gA, gB);

            switch (atime_state) {
                       /* T Y A B Q */
                       /*   t t t   */
            case 0x18: /* 0 1 1 0 0 */ CASE_NAND2_01100          break;
            case 0x1d: /* 0 1 1 1 1 */ CASE_NAND2_01111          break;
            case 0x1e: /* 0 1 1 1 2 */ CASE_NAND2_01112          break;
            case 0x1f: /* 0 1 1 1 3 */ CASE_NAND2_01113          break;

            case 0x34: /* 1 1 0 1 0 */ CASE_NAND2_11010          break;
            case 0x3d: /* 1 1 1 1 1 */ CASE_NAND2_11111          break;
            case 0x3e: /* 1 1 1 1 2 */ CASE_NAND2_11112          break;
            case 0x3f: /* 1 1 1 1 3 */ CASE_NAND2_11113          break;

            case 0x40: /* 2 0 0 0 0 */                           break;
            case 0x44: /* 2 0 0 1 0 */                           break;
            case 0x48: /* 2 0 1 0 0 */                           break;
            case 0x4d: /* 2 0 1 1 1 */ CASE_NAND2_20111          break;
            case 0x4e: /* 2 0 1 1 2 */ CASE_NAND2_20112          break;
            case 0x4f: /* 2 0 1 1 3 */ ASSIGN_YTT_FROM(gA)       break;
            case 0x5d: /* 2 1 1 1 1 */                           break;
            case 0x5e: /* 2 1 1 1 2 */                           break;
            case 0x5f: /* 2 1 1 1 3 */                           break;
            default: assert(0 && "Unknown arival time state.");  break;
            }

            break;
        } // NAND2

        case Module::NOR2: {
            Gate* gA = gate->froms[0];
            Gate* gB = gate->froms[1];

            uint8_t value_state = pack_value(gate, gA, gB);
            
            switch (value_state) {
                       /* Y A B */
            case 0x00: /* 0 0 0 */ return false;              break;
            case 0x01: /* 0 0 1 */                            break;
            case 0x02: /* 0 0 2 */ ASSIGN_B(1)                break;
            case 0x04: /* 0 1 0 */                            break;
            case 0x05: /* 0 1 1 */                            break;
            case 0x06: /* 0 1 2 */                            break;
            case 0x08: /* 0 2 0 */ ASSIGN_A(1)                break;
            case 0x09: /* 0 2 1 */                            break;
            case 0x0a: /* 0 2 2 */                            break;
                
            case 0x10: /* 1 0 0 */                            break;
            case 0x11: /* 1 0 1 */ return false;              break;
            case 0x12: /* 1 0 2 */ ASSIGN_B(0)                break;
            case 0x14: /* 1 1 0 */ return false;              break;
            case 0x15: /* 1 1 1 */ return false;              break;
            case 0x16: /* 1 1 2 */ return false;              break;
            case 0x18: /* 1 2 0 */ ASSIGN_A(0)                break;
            case 0x19: /* 1 2 1 */ return false;              break;
            case 0x1a: /* 1 2 2 */ ASSIGN_A(0) ASSIGN_B(0)    break;

            case 0x20: /* 2 0 0 */ ASSIGN_Y(1)                break;
            case 0x21: /* 2 0 1 */ ASSIGN_Y(0)                break;
            case 0x22: /* 2 0 2 */                            break;
            case 0x24: /* 2 1 0 */ ASSIGN_Y(0)                break;
            case 0x25: /* 2 1 1 */ ASSIGN_Y(0)                break;
            case 0x26: /* 2 1 2 */ ASSIGN_Y(0)                break;
            case 0x28: /* 2 2 0 */                            break;
            case 0x29: /* 2 2 1 */ ASSIGN_Y(0)                break;
            case 0x2a: /* 2 2 2 */                            break;
            default: assert(0 && "Unknown value state.");     break;
            }

            uint8_t atime_state = pack_atime(gate, gA, gB);

            switch (atime_state) {
                       /* T Y A B Q */
                       /*   t t t   */
            case 0x18: /* 0 1 1 0 0 */ CASE_NOR2_01100           break;
            case 0x1d: /* 0 1 1 1 1 */ CASE_NOR2_01111           break;
            case 0x1e: /* 0 1 1 1 2 */ CASE_NOR2_01112           break;
            case 0x1f: /* 0 1 1 1 3 */ CASE_NOR2_01113           break;

            case 0x34: /* 1 1 0 1 0 */ CASE_NOR2_11010           break;
            case 0x3d: /* 1 1 1 1 1 */ CASE_NOR2_11111           break;
            case 0x3e: /* 1 1 1 1 2 */ CASE_NOR2_11112           break;
            case 0x3f: /* 1 1 1 1 3 */ CASE_NOR2_11113           break;

            case 0x40: /* 2 0 0 0 0 */                           break;
            case 0x44: /* 2 0 0 1 0 */                           break;
            case 0x48: /* 2 0 1 0 0 */                           break;
            case 0x4d: /* 2 0 1 1 1 */ CASE_NOR2_20111           break;
            case 0x4e: /* 2 0 1 1 2 */ CASE_NOR2_20112           break;
            case 0x4f: /* 2 0 1 1 3 */ ASSIGN_YTT_FROM(gA)       break;
            case 0x5d: /* 2 1 1 1 1 */                           break;
            case 0x5e: /* 2 1 1 1 2 */                           break;
            case 0x5f: /* 2 1 1 1 3 */                           break;
            default: assert(0 && "Unknown arival time state.");  break;
            }

            break;
        } // NOR2

        case Module::NOT1: {
            Gate* gA = gate->froms[0];

            uint8_t value_state = pack_value(gate, gA);
            
            switch (value_state) {
                      /* Y A */
            case 0x0: /* 0 0 */ return false;              break;
            case 0x1: /* 0 1 */                            break;
            case 0x2: /* 0 2 */ ASSIGN_A(1)                break;
            case 0x4: /* 1 0 */                            break;
            case 0x5: /* 1 1 */ return false;              break;
            case 0x6: /* 1 2 */ ASSIGN_A(0)                break;
            case 0x8: /* 2 0 */ ASSIGN_Y(1)                break;
            case 0x9: /* 2 1 */ ASSIGN_Y(0)                break;
            case 0xa: /* 2 2 */                            break;
            default: assert(0 && "Unknown value state.");  break;
            }

            uint8_t atime_state = pack_atime(gate, gA);

            switch (atime_state) {
                      /* T Y A */
                      /*   t t */
            case 0x3: /* 0 1 1 */                                break;
            case 0x8: /* 2 0 0 */                                break;
            case 0x9: /* 2 0 1 */ ASSIGN_YTT_FROM(gA)            break;
            case 0xb: /* 2 1 1 */                                break;
            default: assert(0 && "Unknown arival time state.");  break;
            }

            break;
        } // NOT1

        case Module::PI: {
            if (gate->arrival_time == UNKNOWN) {
                gate->arrival_time = 0;
                PUSH_FANOUT_TO_QUEUE
            }

            break;
        } // PI

        case Module::PO: {
            Gate* gA = gate->froms[0];

            gate->arrival_time = gA->arrival_time;
            gate->value        = gA->value;
            break;
        } // PO

        default:
            assert(false && "Gate type not recognized.");
            break;

        } // switch (gate->module)

    } // while q is not empty

    // Give default value to PI that has unknown value.
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        if (cir.primary_inputs[i]->value == 2) {
            cir.primary_inputs[i]->value = 0;
        }
    }

    // Reset is_true_path.
    for (size_t i = 0; i < path.size(); ++i) {
        path[i]->is_true_path = false;
    }

    return true;
}

