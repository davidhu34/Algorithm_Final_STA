#include <cctype>
#include <iostream>
#include <fstream>
#include <vector>

#include "cir/circuit.h"
#include "util/hash_map.h"
#include "util/hasher.h"

// Helper function for parser.cpp.

namespace { 
    // Helper for building circuit.
    struct Wire { 
        std::string             name;
        Cir::Gate*              from;
        std::vector<Cir::Gate*> tos;

        // Constructor
        explicit Wire(const std::string& _name): name(_name), from(0) { }
    };

    // TODO: Temporary string hasher.
    size_t hash_str(const std::string& str) {
        return 0;
    }
}

// Free all gates allocated in circuit.
//
// #### Input
//
// - `cir`: Circuit to be freed.
//
static void clear_circuit(Cir::Circuit& cir) {
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        delete cir.primary_inputs[i];
    }
    cir.primary_inputs.clear();

    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        delete cir.primary_outputs[i];
    }
    cir.primary_outputs.clear();

    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        delete cir.logic_gates[i];
    }
    cir.logic_gates.clear();
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

// Parse module NAND2.
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
static int parse_module_NAND2(std::istream& fin, Cir::Circuit& cir) {
    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "nand") {
            while (get_token(fin) != "(") { }; // Ignore until "(".

            token = get_token(fin); // Name of output pin.
            cir.module_NAND2.output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.module_NAND2.input_names.push_back(token);

            token = get_token(fin); // ","

            token = get_token(fin); // Name of input pin.
            cir.module_NAND2.input_names.push_back(token);

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

// Parse module NOR2.
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
static int parse_module_NOR2(std::istream& fin, Cir::Circuit& cir) {
    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "nor") {
            while (get_token(fin) != "(") { }; // Ignore until "("

            token = get_token(fin); // Name of output pin.
            cir.module_NOR2.output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.module_NOR2.input_names.push_back(token);

            token = get_token(fin); // ","

            token = get_token(fin); // Name of input pin.
            cir.module_NOR2.input_names.push_back(token);

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

// Parse module NOT1.
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
static int parse_module_NOT1(std::istream& fin, Cir::Circuit& cir) {
    while (get_token(fin) != ";") { } // Ignore until ";".

    while (true) { // Parse things inside module definition.
        std::string token = get_token(fin);

        if (token == "not") {
            while (get_token(fin) != "(") { }; // Ignore until "(".

            token = get_token(fin); // Name of output pin.
            cir.module_NOT1.output_name = token;

            token = get_token(fin); // ","
            
            token = get_token(fin); // Name of input pin.
            cir.module_NOT1.input_names.push_back(token);

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
static int parse_netlist_module(std::istream& fin, Cir::Circuit& cir) {
    // Use hash_map for quick access using string as index.
    typedef size_t (*Hasher)(const std::string&);
    Util::hash_map<std::string, Wire, Hasher> wires(hash_str);

    size_t number_of_wire = 0;

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
                ++number_of_wire;

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
                wires.insert(token, wire);
                ++number_of_wire;

                token = get_token(fin); // "," or ";"
            } while (token != ";");
        }
        else if (token == "wire") { // Parse wires.
            do {
                token = get_token(fin); // Name of wire.
                wires.insert(token, (Wire(token))); // Add wire.
                ++number_of_wire;
                token = get_token(fin); // "," or ";"
            } while (token != ";");
        }
        else if (token == "NAND2") {
            // Rehash if wires is too cramp, i.e. too many wire cramp in
            // one bucket.
            if (wires.bucket.size() < number_of_wire) {
                wires.rehash(number_of_wire * 2);
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
                
                if (pin_name == cir.module_NAND2.input_names[0]) {
                    wires[token].tos.push_back(gate);
                }
                else if (pin_name == cir.module_NAND2.input_names[1]) {
                    wires[token].tos.push_back(gate);
                }
                else if (pin_name == cir.module_NAND2.output_name) {
                    wires[token].from = gate;
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
            if (wires.bucket.size() < number_of_wire) {
                wires.rehash(number_of_wire * 2);
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
                
                if (pin_name == cir.module_NOR2.input_names[0]) {
                    wires[token].tos.push_back(gate);
                }
                else if (pin_name == cir.module_NOR2.input_names[1]) {
                    wires[token].tos.push_back(gate);
                }
                else if (pin_name == cir.module_NOR2.output_name) {
                    wires[token].from = gate;
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
            if (wires.bucket.size() < number_of_wire) {
                wires.rehash(number_of_wire * 2);
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
                
                if (pin_name == cir.module_NOT1.input_names[0]) {
                    wires[token].tos.push_back(gate);
                }
                else if (pin_name == cir.module_NOT1.output_name) {
                    wires[token].from = gate;
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

        // Connect gates.
        for (size_t i = 0; i < wires.bucket.size(); ++i) {
            for (size_t j = 0; j < wires.bucket[i].size(); ++j) {
                const Wire& wire = wires.bucket[i][j].value;

                // Connect "from" of wire and "to" of wire.
                for (size_t k = 0; k < wire.tos.size(); ++k) { 
                    wire.from  ->tos  .push_back(wire.tos[k]);
                    wire.tos[k]->froms.push_back(wire.from  );
                }
            }
        }
    }

    return 0;
}

