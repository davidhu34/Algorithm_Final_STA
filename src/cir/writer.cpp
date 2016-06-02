#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "sta/src/cir/writer.h"

// Some type setting variables.
static const char* hline = // Horizontal line
    "------------------------------------------------------------\n";
static const int pin_fw     = 10; // field width
static const int type_fw    = 25;
static const int incr_fw    = 10;
static const int delay_fw   = 10;
static const int value_fw   = 5;
static const int timelbl_w  = 20;
static const int timeval_w  = 10;

// Print two gates.
static void print_connection(int                      delay,
                             const Sta::Cir::Gate*    g1,
                             bool                     g1_val,
                             const Sta::Cir::Gate*    g2,
                             bool                     g2_val,
                             const Sta::Cir::Circuit& cir,
                             std::ostream&            out) {
    using namespace Sta::Cir;
    using std::setw;

    // Print g1
    std::ostringstream sout;
    sout << g1->name;

    if (g1->module == Module::NAND2 || 
        g1->module == Module::NOR2  ||
        g1->module == Module::NOT1    ) {

        sout << "/" << cir.modules[g1->module].output_name;
    }

    out << setw(pin_fw)   << sout.str()
        << setw(type_fw)  << "(" + cir.modules[g1->module].name + ")"
        << setw(incr_fw)  << (g1->module == Module::PI ? 0 : 1)
        << setw(delay_fw) << delay
        << setw(value_fw) << (g1_val ? "r" : "f") << "\n";

    // Print g2
    sout.clear();
    sout.str("");
    sout << g2->name;

    if (g2->module == Module::NAND2 || 
        g2->module == Module::NOR2  ||
        g2->module == Module::NOT1    ) {

        for (size_t i = 0; i < g2->froms.size(); ++i) {
            if (g2->froms[i] == g1) {
                sout << "/" << cir.modules[g2->module].input_names[i];
                break;
            }
        }
    }

    out << setw(pin_fw)   << sout.str()
        << setw(type_fw)  << "(" + cir.modules[g2->module].name + ")"
        << setw(incr_fw)  << 0
        << setw(delay_fw) << delay
        << setw(value_fw) << (g1_val ? "r" : "f") << "\n";
}

int Sta::Cir::write(
    const Circuit&                          cir,
    int                                     time_constraint,
    int                                     slack_constraint,
    const std::vector<Path>&                paths,
    const std::vector< std::vector<bool> >& values,
    const std::vector<InputVec>&            input_vecs,
    const std::string&                      outfile          ) {

    // Output to file or stdout depends on outfile.
    //
    std::streambuf* buf;
    std::ofstream fout;

    if (outfile.empty()) {
        buf = std::cout.rdbuf();
    }
    else {
        fout.open(outfile.c_str());
        if (!fout.good()) {
            std::cerr << "Error: Cannot open file " << outfile << ".\n";
            return 1;
        }
        buf = fout.rdbuf();
    }

    std::ostream out(buf);

    out << std::left;

    // Start printing.
    
    out << "Header { A True Path Set }\n\n"
        << "Benchmark { " << cir.name << " }\n\n";

    for (size_t i = 0; i < paths.size(); ++i) {
        out << "Path { " << i + 1 << " }\n\n";

        // Print path
        out << "A True Path List\n"
            << "{\n"
            << hline
            << std::setw(pin_fw)   << "Pin"
            << std::setw(type_fw)  << "Type"
            << std::setw(incr_fw)  << "Incr"
            << std::setw(delay_fw) << "Path Delay"
            << std::setw(value_fw) << "" << "\n"
            << hline;

        for (size_t j = paths[i].size(); j > 1; --j) {
            print_connection(paths[i].size() - j,
                             paths[i][j - 1],
                             values[i][j - 1],
                             paths[i][j - 2],
                             values[i][j - 2],
                             cir,
                             out                 );
        }

        int delay = paths[i].size() - 2;

        out << hline
            << std::setw(timelbl_w) << "Data Required Time" << std::right
            << std::setw(timeval_w) << time_constraint << "\n" << std::left
            << std::setw(timelbl_w) << "Data Arrival Time" << std::right
            << std::setw(timeval_w) << delay << "\n" << std::left
            << hline
            << std::setw(timelbl_w) << "Slack" << std::right
            << std::setw(timeval_w) << time_constraint - delay << "\n"
            << "}\n\n" << std::left;
        
        // Print input vector
        out << "Input Vector\n"
            << "{\n";

        for (size_t j = 0; j < input_vecs[i].size(); ++j) {
            out << std::setw(13) << cir.primary_inputs[j]->name
                << std::setw(2)  << "="
                << std::setw(15);

            if (paths[i].back() != cir.primary_inputs[j]) {
                out << static_cast<int>(input_vecs[i][j]);
            }
            else {
                out << (input_vecs[i][j] ? "r" : "f");
            }
            out << "\n";
        }

        out << "}\n\n";
    }
    out << std::right;

    return 0;
}
