#include "sta/src/ana/analyzer.h"

#include "sta/src/ana/helper.h"

// `g1` is online signal, `g2` is side input.
//
static uint8_t pack_state(const Sta::Cir::Gate* gY,
                          const Sta::Cir::Gate* g1,
                          const Sta::Cir::Gate* g2 ) {

                      /*    +------ value of gY      */
                      /*    | +---- value of g1      */
                      /*    | |+--- value of g2      */
                      /*    | ||+-- g1 arrived first */
                      /*    | |||+- g2 arrived first */
                      /*    | ||||                   */
    uint8_t bits = 0; /* 000X XXXX                   */

    bits |= gY->value;
    bits <<= 1;
    bits |= g1->value;
    bits <<= 1;
    bits |= g2->value;
    bits <<= 2;
    
    if (g1->arrival_time < g2->arrival_time) {
        bits |= 2;
    }
    else if (g1->arrival_time > g2->arrival_time) {
        bits |= 1;
    }
    else { /* g1->arrival_time == g2->arrival_time */
        bits |= 3;
    }

    return bits;
}

bool Sta::Ana::verify_true_path(const Cir::Circuit&  cir,
                                const Cir::Path&     path,
                                const Cir::InputVec& input_vec,
                                const Cir::Gate*&    g1,
                                const Cir::Gate*&    g2,
                                const Cir::Gate*&    gY        ) {
    using Cir::Module;
    
    calculate_value_and_arrival_time(cir, input_vec);

    for (size_t i = 0; i < path.size() - 1; ++i) {
        g1 = path[i];
        g2 = 0;
        gY = path[i + 1];

        switch (gY->module) {
        case Module::NAND2: {
            if (g1 == gY->froms[0]) {
                g2 = gY->froms[1];
            }
            else {
                g2 = gY->froms[0];
            }

            uint8_t bits = pack_state(gY, g1, g2);

            switch (bits) {
            case 0x0d /* 0 1 1 1 */:
            case 0x0f /* 0 1 1 3 */:
            case 0x12 /* 1 0 0 2 */:
            case 0x13 /* 1 0 0 3 */:
            case 0x16 /* 1 0 1 2 */:
            case 0x15 /* 1 0 1 1 */:
            case 0x17 /* 1 0 1 3 */:
                continue;
                break;
            }

            return false;
            break;
        }
        case Module::NOR2: {
            if (g1 == gY->froms[0]) {
                g2 = gY->froms[1];
            }
            else {
                g2 = gY->froms[0];
            }

            uint8_t bits = pack_state(gY, g1, g2);

            switch (bits) {
            case 0x0a /* 0 1 0 2 */:
            case 0x09 /* 0 1 0 1 */:
            case 0x0b /* 0 1 0 3 */:
            case 0x0e /* 0 1 1 2 */:
            case 0x0f /* 0 1 1 3 */:
            case 0x11 /* 1 0 0 1 */:
            case 0x13 /* 1 0 0 3 */:
                continue;
                break;
            }

            return false;
            break;
        }
        } // switch
    }
    return true;
}

bool Sta::Ana::verify_true_path(const Cir::Circuit&  cir,
                                const Cir::Path&     path,
                                const Cir::InputVec& input_vec) {
    using Cir::Gate;

    const Gate* d1; // dummy
    const Gate* d2; // dummy
    const Gate* d3; // dummy

    return verify_true_path(cir, path, input_vec, d1, d2, d3);
}
