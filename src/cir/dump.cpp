#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "sta/src/cir/writer.h"

bool Sta::Cir::dump(const Circuit&     cir,
                    const std::string& dump_file) {

    using std::setw;

    // Output to file or stdout depends on dump_file.
    //
    std::streambuf* buf;
    std::ofstream   fout;

    if (dump_file.empty()) {
        buf = std::cout.rdbuf();
    }
    else {
        fout.open(dump_file.c_str());
        if (!fout.good()) {
            std::cerr << "Error: Cannot open file " << dump_file
                      << ".\n";
            return false;
        }
        buf = fout.rdbuf();
    }

    std::ostream out(buf);
    const int    w = 30; // Field width.

    out << std::left;

    // Start printing.
    out << setw(w) << "-circuit_name" << cir.name << "\n"
        << setw(w) << "-pi_count" << cir.primary_inputs.size() << "\n"
        << setw(w) << "-po_count" << cir.primary_outputs.size() << "\n"
        << setw(w) << "-gate_count" << cir.logic_gates.size() << "\n"
        << setw(w) << "-nand2_input_pin_name" 
                   << cir.modules[Module::NAND2].input_names[0] << " " 
                   << cir.modules[Module::NAND2].input_names[1] << "\n" 
        << setw(w) << "-nand2_output_pin_name"
                   << cir.modules[Module::NAND2].output_name << "\n"
        << setw(w) << "-nor2_input_pin_name" 
                   << cir.modules[Module::NOR2].input_names[0] << " " 
                   << cir.modules[Module::NOR2].input_names[1] << "\n" 
        << setw(w) << "-nor2_output_pin_name"
                   << cir.modules[Module::NOR2].output_name << "\n"
        << setw(w) << "-not1_input_pin_name" 
                   << cir.modules[Module::NOT1].input_names[0] << "\n"
        << setw(w) << "-not1_output_pin_name"
                   << cir.modules[Module::NOT1].output_name << "\n";

    std::string temp;
    temp.reserve(64);

    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        const Gate* g = cir.logic_gates[i];
        // Print fanin
        temp.clear();
        temp += "-";
        temp += g->name;
        temp += "/";
        temp += cir.modules[g->module].name;

        out << setw(w) << temp + "/fanin";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            out << cir.modules[g->module].input_names[j] << ":"
                << g->froms[j]->name << " ";
        }
        out << "\n";

        // Print fanout
        out << setw(w) << temp + "/fanout";

        for (size_t j = 0; j < g->tos.size(); ++j) {
            out << cir.modules[g->module].output_name << ":"
                << g->tos[j]->name << " ";
        }
        out << "\n";
    }

    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        // Print fanin
        const Gate* g = cir.primary_outputs[i];
        out << setw(w) << "-" + g->name + "/PO/fanin";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            out << g->froms[j]->name << " ";
        }
        out << "\n";
    }

    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        // Print fanout
        const Gate* g = cir.primary_inputs[i];
        out << setw(w) << "-" + g->name + "/PI/fanout";

        for (size_t j = 0; j < g->tos.size(); ++j) {
            out << g->tos[j]->name << " ";
        }
        out << "\n";
    }

    return true;
}
