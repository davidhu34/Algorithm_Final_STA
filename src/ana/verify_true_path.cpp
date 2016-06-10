#include "sta/src/ana/analyzer.h"

int Sta::Ana::verify_true_path(Cir::Circuit&        cir,
                               const Cir::Path&     path,
                               const Cir::InputVec& input_vec,
                               const Cir::Gate*&    g1,
                               const Cir::Gate*&    g2,
                               const Cir::Gate*&    gY        ) {
    using Cir::Gate;
    using Cir::Module;
    
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        cir.primary_inputs[i]->value = input_vec[i];
    }
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        cir.primary_outputs[i]->value = 2;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        cir.logic_gates[i]->value = 2;
    }

    calculate_value_and_arrival_time(cir);

    for (size_t i = 0; i < path.size() - 1; ++i) {
        g1 = path[i];
        g2 = 0;
        gY = path[i + 1];

        switch (gY->module) {
        case Module::NAND2: {

        #define CHECK_GATE(v0, v1, afirst, bfirst)                      \
            /* Try to find side input */                                \
            if (gY->froms[0] == g1) {                                   \
                g2 = gY->froms[1];                                      \
            }                                                           \
            else {                                                      \
                g2 = gY->froms[0];                                      \
            }                                                           \
                                                                        \
            uint8_t bits = 0; /* 000X XXXX                   */         \
                              /*    | ||||                   */         \
                              /*    | |||+- g2 arrived first */         \
                              /*    | ||+-- g1 arrived first */         \
                              /*    | |+--- value of g2      */         \
                              /*    | +---- value of g1      */         \
                              /*    +------ value of gY      */         \
                                                                        \
            bits |= (gY->value << 4);                                   \
            bits |= (g1->value << 3);                                   \
            bits |= (g2->value << 2);                                   \
                                                                        \
            if (g1->arrival_time < g2->arrival_time) {                  \
                bits |= 2;                                              \
            }                                                           \
            else if (g1->arrival_time > g2->arrival_time) {             \
                bits |= 1;                                              \
            }                                                           \
            else { /* g1->arrival_time == g2->arrival_time */           \
                bits |= 3;                                              \
            }                                                           \
                                                                        \
            if (bits == ((0  << 4) | (1  << 3) | (1  << 2) | bfirst) || \
                bits == ((0  << 4) | (1  << 3) | (1  << 2) | 3     ) || \
                bits == ((1  << 4) | (0  << 3) | (0  << 2) | afirst) || \
                bits == ((1  << 4) | (0  << 3) | (0  << 2) | 3     ) || \
                bits == ((v1 << 4) | (v0 << 3) | (v1 << 2) | 2     ) || \
                bits == ((v1 << 4) | (v0 << 3) | (v1 << 2) | 1     ) || \
                bits == ((v1 << 4) | (v0 << 3) | (v1 << 2) | 3     ))  {\
                                                                        \
                return 1;                                               \
            }
            
            CHECK_GATE(0, 1, 2, 1)
            break;
        }
        case Module::NOR2: {
            CHECK_GATE(1, 0, 1, 2)
            break;
        }
        } // switch
    }
    return 0;
}

int Sta::Ana::verify_true_path(Cir::Circuit&        cir,
                               const Cir::Path&     path,
                               const Cir::InputVec& input_vec) {
    using Cir::Gate;

    const Gate* d1; // dummy
    const Gate* d2; // dummy
    const Gate* d3; // dummy

    return verify_true_path(cir, path, input_vec, d1, d2, d3);
}
