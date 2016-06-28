#include "sta/src/cir/parser.h"

#include <fstream>
#include <vector>

#include "sta/src/cir/circuit.h"
#include "sta/src/util/hash_map.h"

// Include helper functions for parser.cpp
#include "sta/src/cir/parse.inc"

bool Sta::Cir::parse(const std::vector<const char*>& input_files,
                     Circuit&                        circuit     ) {
    
    // Parse all input files.
    for (size_t i = 0; i < input_files.size(); ++i) {
        std::ifstream fin(input_files[i]);
        if (!fin.good()) {
            std::cerr << "Error: Cannot open file '"
                      << input_files[i] << "'\n";
            return false;
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
                bool success = true;

                token = get_token(fin); // Read module name.

                if (token == "NAND2") {
                    success &= parse_module_NAND2(fin, circuit);
                }
                else if (token == "NOR2") {
                    success &= parse_module_NOR2(fin, circuit);
                }
                else if (token == "NOT1") {
                    success &= parse_module_NOT1(fin, circuit);
                }
                else { // It is the netlist.
                    circuit.name = token;
                    success &= parse_netlist_module(fin, circuit);
                }

                if (!success) { // Error code is not 0.
                    return false;
                }
            }
            else { // Token not recognized or not important,
                   // just ignore it.
            }
        }
    }

    return true;
}

bool Sta::Cir::parse(const char* in_file_1,
                     Circuit&    circuit     ) {

    std::vector<const char*> in_files(1, in_file_1);

    return parse(in_files, circuit);
}

bool Sta::Cir::parse(const char* in_file_1,
                     const char* in_file_2,
                     Circuit&    circuit     ) {

    std::vector<const char*> in_files(2);
    in_files[0] = in_file_1;
    in_files[1] = in_file_2;

    return parse(in_files, circuit);
}

bool Sta::Cir::parse(const char* in_file_1,
                     const char* in_file_2,
                     const char* in_file_3,
                     Circuit&    circuit     ) {

    std::vector<const char*> in_files(3);
    in_files[0] = in_file_1;
    in_files[1] = in_file_2;
    in_files[2] = in_file_3;

    return parse(in_files, circuit);
}

