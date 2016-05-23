#include "cir/parser.h"

#include <fstream>
#include <vector>

#include "cir/circuit.h"
#include "util/hash_map.h"

// Include helper functions for parser.cpp
#include "cir/parser.inc"

int Cir::parse(const std::vector<const char*>& input_files,
               Cir::Circuit&                   circuit     ) {
    
    int err_code = 0;

    // Parse all input files.
    for (size_t i = 0; i < input_files.size(); ++i) {
        std::ifstream fin(input_files[i]);
        if (!fin.good()) {
            std::cerr << "Error: Cannot open file '"
                      << input_files[i] << "'\n";
            err_code = 1;
            goto clean_up;
        }

        // Parse token by token, and take action depending on what token
        // we receive.
        while (true) {
            std::string token = get_token(fin);

            if (token == "") { // EOF. This file is read successfully.
                break;
            }
            else if (token == "`") { // Compiler directives, ignore till "\n".
                while (get_token(fin, 1) != "\n") { }
            }
            else if (token == "module") {
                token = get_token(fin); // Read module name.

                if (token == "NAND2") {
                    err_code = parse_module_NAND2(fin, circuit);
                }
                else if (token == "NOR2") {
                    err_code = parse_module_NOR2(fin, circuit);
                }
                else if (token == "NOT1") {
                    err_code = parse_module_NOT1(fin, circuit);
                }
                else { // It is the netlist.
                    err_code = parse_netlist_module(fin, circuit);
                }

                if (err_code) {
                    goto clean_up;
                }
            }
            else { // Token not recognized or not important,
                   // just ignore it.
            }
        }
    }

clean_up:
    clear_circuit(circuit);
    return err_code;
}
