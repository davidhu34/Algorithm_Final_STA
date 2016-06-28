#ifndef STA_CIR_SUBCIRCUIT_H
#define STA_CIR_SUBCIRCUIT_H

#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Cir {

struct GateCopy {
    uint8_t                value;
    uint8_t                tag;
    int                    arrival_time;
    const Gate*            orig;
    std::vector<GateCopy*> froms;
    std::vector<GateCopy*> tos;

    explicit GateCopy(const Gate* g): orig(g) { }
};

typedef std::vector<GateCopy*> PathCopy;

struct Subcircuit {
    const Circuit&         orig;
    std::vector<GateCopy*> primary_inputs;
    std::vector<GateCopy*> primary_outputs;
    std::vector<GateCopy*> logic_gates;

    // Start from g, send a wave toward PI. Add every gate touched
    // by the wave into this subcircuit.
    //
    Subcircuit(const Circuit& cir, const Gate* g);

    // Free all gates allocated in circuit.
    ~Subcircuit();
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_SUBCIRCUIT_H
