#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

#include <stdint.h>
#include <vector>
#include <string>

namespace Sta {
namespace Cir {

struct Module {
    // Type of Module
    enum {
        NAND2 = 0,
        NOR2,
        NOT1,
        PI,    // Primary input.
        PO,    // Primary output.
    };

    enum {
        npos = std::string::npos
    };

    // Data Member
    std::string              name;
    std::string              output_name;
    std::vector<std::string> input_names;

    // Constructor
    explicit Module(const std::string& _name): name(_name) { }

    // Find index of input pin. If not found, return npos.
    size_t find_input_name(const std::string& pin_name) const {
        for (size_t i = 0; i < input_names.size(); ++i) {
            if (input_names[i] == pin_name) {
                return i;
            }
        }
        return npos;
    }
};

struct Gate {
    // Data Member
    uint8_t            module;       // NAND2, NOR2, NOT1, PI or PO
    uint8_t            value;        // 1:true, 0:false or 2:floating.
    uint16_t           tag;          // Needed when using some algo.
    int32_t            arrival_time; 
    int                var;          // SAT variable.
    std::string        name;
    std::vector<Gate*> froms;
    std::vector<Gate*> tos;

    // Constructor
    Gate(int                _module, 
         const std::string& _name   ):
        module (_module),
        name   (_name)    { }
};

// Path type.
typedef std::vector<Gate*> Path;

// Input vector type.
typedef std::vector<uint8_t> InputVec;

struct Circuit {
    // Data Member
    std::string name;

    std::vector<Gate*>  primary_inputs;
    std::vector<Gate*>  primary_outputs;
    std::vector<Gate*>  logic_gates;
    std::vector<Module> modules;

    // Default Constructor
    Circuit() {
        modules.reserve(5);
        modules.push_back(Module("NAND2"));
        modules.push_back(Module("NOR2" ));
        modules.push_back(Module("NOT1" ));
        modules.push_back(Module("in"   ));
        modules.push_back(Module("out"  ));
    }

    // Free all gates allocated in circuit.
    //
    void clear();
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_CIRCUIT_H
