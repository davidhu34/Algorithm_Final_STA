#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

#include <limits.h>
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
        npos = SIZE_MAX
    };

    std::string              name;
    std::string              output_name;
    std::vector<std::string> input_names;

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

namespace Time {
    enum {
        UNKNOWN = INT_MAX
    };
}

struct Gate {
    uint8_t            module;       // NAND2, NOR2, NOT1, PI or PO
    uint8_t            value;        // 1:true, 0:false or 2:floating.
    uint8_t            tag;          // Needed when using some algo.
    int                arrival_time; 
    int                min_arrival_time; 
    int                max_arrival_time; 
    std::string        name;
    std::vector<Gate*> froms;
    std::vector<Gate*> tos;

    Gate(int                _module, 
         const std::string& _name   ):
        module (_module),
        name   (_name)    { }
};

typedef std::vector<Gate*>     Path;
typedef std::vector<bool>      PathValue;
typedef std::vector<bool>      InputVec;

struct Circuit {
    std::string         name;
    std::vector<Gate*>  primary_inputs;
    std::vector<Gate*>  primary_outputs;
    std::vector<Gate*>  logic_gates;
    std::vector<Module> modules;

    Circuit() {
        modules.reserve(5);
        modules.push_back(Module("NAND2"));
        modules.push_back(Module("NOR2" ));
        modules.push_back(Module("NOT1" ));
        modules.push_back(Module("in"   ));
        modules.push_back(Module("out"  ));
    }

    // Performs deep copy.
    Circuit(const Circuit& circuit);

    // Free all gates allocated in circuit.
    ~Circuit();
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_CIRCUIT_H
