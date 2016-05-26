#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

#include <vector>
#include <string>

namespace Cir {

struct Module {
    // Type of Module
    enum {
        NAND2 = 0,
        NOR2,
        NOT1,
        PI,    // Primary input.
        PO     // Primary output.
    };

    // Data Member
    std::string              name;
    std::vector<std::string> input_names;
    std::string              output_name;

    // Constructor
    explicit Module(const std::string& _name): name(_name) { }
};

struct Gate {
    // Data Member
    int                module;       // NAND2, NOR2, NOT1, PI or PO
    std::string        name;
    int                value;        // 1, 0 or -1 (floating).
    int                arrival_time; // -1 represent unknown.
    std::vector<Gate*> froms;
    std::vector<Gate*> tos;

    // Constructor
    Gate(int                _module, 
         const std::string& _name   ):
        module      (_module),
        name        (_name),
        value       (-1),
        arrival_time(-1) { }
};

// Path type.
typedef std::vector<const Cir::Gate*> Path;

// Input vector type.
typedef std::vector<bool> InputVec;

struct Circuit {
    // Data Member
    std::string name;

    std::vector<Gate*> primary_inputs;
    std::vector<Gate*> primary_outputs;
    std::vector<Gate*> logic_gates;
    
    Module modules[3];

    // Default Constructor
    Circuit(): modules({Module("NAND2"), 
                        Module("NOR2" ), 
                        Module("NOT1" )}) { }

    // Free all gates allocated in circuit.
    //
    void clear();
};

} // namespace Cir

#endif // STA_CIR_CIRCUIT_H
