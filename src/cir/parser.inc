#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>

#include "sta/src/cir/circuit.h"
#include "sta/src/util/hash_map.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/prime.h"

// Helper function for parser.cpp.

namespace { 
    // Helper for building circuit.
    struct Wire { 
        std::string                  name;
        Sta::Cir::Gate*              from;
        std::vector<Sta::Cir::Gate*> tos;
        std::vector<int>             tos_pin; // Connect to which pin?

        // Constructor
        explicit Wire(const std::string& _name): name(_name), from(0) { }
    };
}

// Get a token from stream. A token is defined to be:
//
// - Consecutive charecter of alphabet, number, "$" dollar sign and
//   "_" underscore.
//
// - "." dot,           "," comma,             ":" colon,
//   ";" semicolon,     "(" open parenthesis,  ")" close parenthesis,
//   "[" open bracket,  "]" close bracket,     "=" equal sign, 
//   "`" backtick,      "*" asterisk,          "/" slash,
//   "*>" multipath
//
// Other characters are separator, will be ignored. Comment (everything
// after "//" double slash and before a newline "\n", including "//" and
// "\n") will be ignored.
//
// #### Input
//
// - `fin`         : Input file stream.
// - `keep_newline`: Treat newline as a token and return it.
//
// #### Output
//
// - A token. If any error occur, return an empty string.
//
static std::string get_token(std::istream& fin, bool keep_newline = 0) {
    std::string token;
    char        c;

reread:
    if (fin.get(c).good()) { // No error or EOF.
        switch (c) {
            case '.':
            case ',':
            case ':':
            case ';':
            case '(':
            case ')':
            case '[':
            case ']':
            case '=':
            case '`': { // A token.
                token.push_back(c);
                break;
            }
            case '*': {
                token.push_back(c);
                if (fin.get(c).good()) {   // No error or EOF.
                    if (c == '>') {        // It is "*>" token.
                        token.push_back(c);
                    }
                    else {                 // It is just "*" token.
                        fin.putback(c);
                    }
                }
                break;
            }
            case '\n': {
                if (keep_newline) {      // "\n" is token.
                    token.push_back(c);
                }
                else {                   // "\n" is not token.
                    goto reread;
                }
                break;
            }
            case '/': {
                if (fin.get(c).good() && c == '/') { // It is a comment.
                    // Ignore until newline.
                    while (fin.get(c).good() && c != '\n') { }
                    goto reread;
                }
                else {
                    fin.putback(c);
                    token.push_back('/');
                }
                break;
            }
            case ' ':
            case '\t':
            case '\r':
            case '\v': {
                goto reread;
                break;
            }
            default: {
                if (isalnum(c) || c == '_' || c == '$') {
                    token.push_back(c);
                    
                    // Read consecutive characters.
                    while (fin.get(c).good()) {
                        if (isalnum(c) || c == '_' || c == '$') {
                            token.push_back(c);
                        }
                        else {
                            fin.putback(c);
                            break;
                        }
                    }
                }
                else { // Unrecognized character, reread.
                    goto reread;
                }

                break;
            }
        } // switch (c)
    } // if (fin.get(c).good())

    return token;
}

// Parse module NAND2. Just try to get name of pins. Ignore everything
// else.
//
// #### Input
//
// - `fin`: Input file stream.
// - `cir`: Parse into this circuit.
//
// #### Output
//
// - Error code
//     - 0: Successful
//     - 1: Error
//  
static int parse_module_NAND2(std::istream& fin, Sta::Cir::Circuit& cir) {
    using namespace Sta;

    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "nand") {
            while (get_token(fin) != "(") { }; // Ignore until "(".

            token = get_token(fin); // Name of output pin.
            cir.modules[Cir::Module::NAND2].output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.modules[Cir::Module::NAND2].input_names.push_back(token);

            token = get_token(fin); // ","

            token = get_token(fin); // Name of input pin.
            cir.modules[Cir::Module::NAND2].input_names.push_back(token);

            while (get_token(fin) != ";") { } // Ignore until ";".
        }
        else if (token == "endmodule") { // End module definition.
            break;
        }
        else { // Token not recognized or not important, just ignore it.
        }
    }

    return 0;
}

// Parse module NOR2. Just try to get name of pins. Ignore everything
// else.
//
// #### Input
//
// - `fin`: Input file stream.
// - `cir`: Parse into this circuit.
//
// #### Output
//
// - Error code
//     - 0: Successful
//     - 1: Error
//  
static int parse_module_NOR2(std::istream& fin, Sta::Cir::Circuit& cir) {
    using namespace Sta;

    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "nor") {
            while (get_token(fin) != "(") { }; // Ignore until "("

            token = get_token(fin); // Name of output pin.
            cir.modules[Cir::Module::NOR2].output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.modules[Cir::Module::NOR2].input_names.push_back(token);

            token = get_token(fin); // ","

            token = get_token(fin); // Name of input pin.
            cir.modules[Cir::Module::NOR2].input_names.push_back(token);

            while (get_token(fin) != ";") { } // Ignore until ";".
        }
        else if (token == "endmodule") { // End module definition.
            break;
        }
        else { // Token not recognized or not important, just ignore it.
        }
    }

    return 0;
}

// Parse module NOT1. Just try to get name of pins. Ignore everything
// else.
//
// #### Input
//
// - `fin`: Input file stream.
// - `cir`: Parse into this circuit.
//
// #### Output
//
// - Error code
//     - 0: Successful
//     - 1: Error
//  
static int parse_module_NOT1(std::istream& fin, Sta::Cir::Circuit& cir) {
    using namespace Sta;

    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "not") {
            while (get_token(fin) != "(") { }; // Ignore until "(".

            token = get_token(fin); // Name of output pin.
            cir.modules[Cir::Module::NOT1].output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.modules[Cir::Module::NOT1].input_names.push_back(token);

            while (get_token(fin) != ";") { } // Ignore until ";".
        }
        else if (token == "endmodule") { // End module definition.
            break;
        }
        else { // Token not recognized or not important, just ignore it.
        }
    }

    return 0;
}

// Parse netlist module.
//
// Method of parsing circuit: First get a set of PIs, a set of POs, and
// a set of Wires. Then read in each gate. Each line of gate describes
// which pin of gate connect to which wire. If that pin is an input pin,
// put it at wire's "to". If that pin is an output pin, put it at wire's
// "from". After that, connect all 
// 
// #### Input
//
// - `fin`: Input file stream.
// - `cir`: Parse into this circuit.
//
// #### Output
//
// - Error code
//     - 0: Successful
//     - 1: Error
//  
static int parse_netlist_module(std::istream& fin, Sta::Cir::Circuit& cir) {
    using namespace Sta;

    // Use hash_map for quick access using string as index.
    typedef uint32_t (*Hasher)(const std::string&);
    Util::HashMap<std::string, Wire, Hasher> wires(Util::hash_str);

    // Start parsing.

    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "input") { // Parse input pins.
            do {
                token = get_token(fin); // Name of input pin.

                // Add new PI into circuit.
                Cir::Gate* pi = new Cir::Gate(Cir::Module::PI, token);
                cir.primary_inputs.push_back(pi);

                // Connect that PI to wire with the same name, so that
                // we can connect PI to gate later.
                Wire wire(token);
                wire.from = pi;
                wires.insert(token, wire);

                token = get_token(fin); // "," or ";"
            } while (token != ";");
        }
        else if (token == "output") { // Parse output pins.
            do {
                token = get_token(fin); // Name of output pin.

                // Add new PO into circuit.
                Cir::Gate* po = new Cir::Gate(Cir::Module::PO, token);
                cir.primary_outputs.push_back(po);

                // Connect that PO to wire with the same name, so that
                // we can connect PO to gate later.
                Wire wire(token);
                wire.tos.push_back(po);
                wire.tos_pin.push_back(0);
                wires.insert(token, wire);

                token = get_token(fin); // "," or ";"
            } while (token != ";");
        }
        else if (token == "wire") { // Parse wires.
            do {
                token = get_token(fin); // Name of wire.
                wires.insert(token, (Wire(token))); // Add wire.
                token = get_token(fin); // "," or ";"
            } while (token != ";");
        }
        else if (token == "NAND2") {
            // Rehash if wires is too cramp, i.e. too many wire cramp in
            // one bucket.
            if (wires.bucket.size() < wires.size) {
                wires.rehash(Util::prime_gt(wires.size * 2));
            }

            token = get_token(fin); // Name of gate.

            // Add new NAND2 gate to circuit.
            Cir::Gate* gate = new Cir::Gate(Cir::Module::NAND2, token);
            cir.logic_gates.push_back(gate);

            token = get_token(fin); // "("

            do { // Parse arguments.
                token                = get_token(fin); // "."
                std::string pin_name = get_token(fin); // Pin name.
                token                = get_token(fin); // "("
                token                = get_token(fin); // Wire name.
                
                const Cir::Module* mod = &cir.modules[Cir::Module::NAND2];
                Wire&              w   = wires[token];

                if (pin_name == mod->input_names[0]) {
                    w.tos.push_back(gate);
                    w.tos_pin.push_back(0);
                }
                else if (pin_name == mod->input_names[1]) {
                    w.tos.push_back(gate);
                    w.tos_pin.push_back(1);
                }
                else if (pin_name == mod->output_name) {
                    w.from = gate;
                }
                else {
                    std::cerr << "Error: Pin '" << token
                              << "' not recognized.\n";
                    return 1;
                }

                token = get_token(fin); // ")"
                token = get_token(fin); // "," or ")"
            } while (token != ")");

            token = get_token(fin); // ";"
        }
        else if (token == "NOR2") {
            // Rehash if wires is too cramp, i.e. too many wire cramp in
            // one bucket.
            if (wires.bucket.size() < wires.size) {
                wires.rehash(Util::prime_gt(wires.size * 2));
            }

            token = get_token(fin); // Name of gate.

            // Add new NOR2 gate to circuit.
            Cir::Gate* gate = new Cir::Gate(Cir::Module::NOR2, token);
            cir.logic_gates.push_back(gate);

            token = get_token(fin); // "("

            do { // Parse arguments.
                token                = get_token(fin); // "."
                std::string pin_name = get_token(fin); // Pin name.
                token                = get_token(fin); // "("
                token                = get_token(fin); // Wire name.
                
                const Cir::Module* mod = &cir.modules[Cir::Module::NOR2];
                Wire&              w   = wires[token];

                if (pin_name == mod->input_names[0]) {
                    w.tos.push_back(gate);
                    w.tos_pin.push_back(0);
                }
                else if (pin_name == mod->input_names[1]) {
                    w.tos.push_back(gate);
                    w.tos_pin.push_back(1);
                }
                else if (pin_name == mod->output_name) {
                    w.from = gate;
                }
                else {
                    std::cerr << "Error: Pin '" << token
                              << "' not recognized.\n";
                    return 1;
                }

                token = get_token(fin); // ")"
                token = get_token(fin); // "," or ")"
            } while (token != ")");

            token = get_token(fin); // ";"
        }
        else if (token == "NOT1") {
            // Rehash if wires is too cramp, i.e. too many wire cramp in
            // one bucket.
            if (wires.bucket.size() < wires.size) {
                wires.rehash(Util::prime_gt(wires.size * 2));
            }

            token = get_token(fin); // Name of gate.

            // Add new NOT1 gate to circuit.
            Cir::Gate* gate = new Cir::Gate(Cir::Module::NOT1, token);
            cir.logic_gates.push_back(gate);

            token = get_token(fin); // "("

            do { // Parse arguments.
                token                = get_token(fin); // "."
                std::string pin_name = get_token(fin); // Pin name.
                token                = get_token(fin); // "("
                token                = get_token(fin); // Wire name.
                
                const Cir::Module* mod = &cir.modules[Cir::Module::NOT1];
                Wire&              w   = wires[token];

                if (pin_name == mod->input_names[0]) {
                    w.tos.push_back(gate);
                    w.tos_pin.push_back(0);
                }
                else if (pin_name == mod->output_name) {
                    w.from = gate;
                }
                else {
                    std::cerr << "Error: Pin '" << token
                              << "' not recognized.\n";
                    return 1;
                }

                token = get_token(fin); // ")"
                token = get_token(fin); // "," or ")"
            } while (token != ")");

            token = get_token(fin); // ";"
        }
        else if (token == "endmodule") { // End module definition.
            break;
        }
        else { // Token not recognized or not important, just ignore it.
        }
    }

    // Connect gates.
    for (size_t i = 0; i < wires.bucket.size(); ++i) {
        for (size_t j = 0; j < wires.bucket[i].size(); ++j) {
            const Wire& w = wires.bucket[i][j].value;

            // Connect "from" of w and "to" of w.
            for (size_t k = 0; k < w.tos.size(); ++k) { 

                // If w if not PI
                if (w.from) {
                    w.from->tos.push_back(w.tos[k]);

                    if (w.tos[k]->froms.size() < w.tos_pin[k] + 1) {
                        w.tos[k]->froms.resize(w.tos_pin[k] + 1);
                    }
                    w.tos[k]->froms[w.tos_pin[k]] = w.from;
                }
            }
        }
    }

    return 0;
}

