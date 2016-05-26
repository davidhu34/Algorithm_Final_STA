#include "cir/circuit.h"

void Cir::Circuit::clear() {
    for (size_t i = 0; i < primary_inputs.size(); ++i) {
        delete primary_inputs[i];
    }
    primary_inputs.clear();

    for (size_t i = 0; i < primary_outputs.size(); ++i) {
        delete primary_outputs[i];
    }
    primary_outputs.clear();

    for (size_t i = 0; i < logic_gates.size(); ++i) {
        delete logic_gates[i];
    }
    logic_gates.clear();

    for (size_t i = 0; i < modules.size(); ++i) {
        modules[i].input_names.clear();
        modules[i].output_name.clear();
    }
}
