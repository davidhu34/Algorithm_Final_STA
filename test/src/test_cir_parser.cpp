#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/test/src/util/util.h"
#include "sta/test/src/util/cir_compare.h"

// Helper functions

// Only do content independent test, i.e. it should be true for all
// circuit.
//
// - All module has correct number of input and output names:
//     - Module NAND2 has 2 input names and 1 output name.
//     - Module NOR2 has 2 input names and 1 output name.
//     - Module NOT1 has 1 input name and 1 output name.
// - Circuit has name.
// - All node has correct type.
// - All node has name.
// - All node has correct number of input and output:
//     - All NAND2 gate has 2 inputs and 1 or more output.
//     - All NOR2 gate has 2 inputs and 1 or more output.
//     - All NOT1 gate has 1 input and 1 or more output.
//     - All PI has 0 input and 1 or more output.
//     - All PO has 1 input and 0 output.
// - All edge is bidirectional (If A point to B, then B point from A).
//
// #### Input
//
// - `files`:
//
// #### Output
//
// - Parsed circuit.
//
static void basic_validate(const std::vector<const char*>& files,
                           Sta::Cir::Circuit& cir) {

    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

    int ret = Cir::parse(files, cir);
    ASSERT(!ret, << "Cir::parse() return " << ret << "\n");

    // Make sure all modules has correct number of input and
    // output names.
    
    const Cir::Module* mod = &cir.modules[Cir::Module::NAND2];
    ASSERT(mod->input_names.size() == 2,
        << "Module NAND2 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    mod = &cir.modules[Cir::Module::NOR2];
    ASSERT(mod->input_names.size() == 2,
        << "Module NOR2 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    mod = &cir.modules[Cir::Module::NOT1];
    ASSERT(mod->input_names.size() == 1,
        << "Module NOT1 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    // Make sure circuit has name.
    ASSERT(cir.name != "", );

    // For each pi in primary_inputs,
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        const Cir::Gate* pi = cir.primary_inputs[i];

        // Make sure PI has correct type.
        ASSERT(pi->module == Cir::Module::PI, 
            << "PI " << pi->name << " has type " << pi->module << "\n");

        // Make sure PI has name.
        ASSERT(pi->name != "", );

        // Make sure PI has 0 input.
        ASSERT(pi->froms.size() == 0,
            << "PI " << pi->name << " has " << pi->froms.size()
            << " input.\n");

        // Make sure PI has 1 or more output.
        ASSERT(pi->tos.size() > 0,
            << "PI " << pi->name << " has " << pi->tos.size()
            << " input.\n");

        // Make sure all edge is bidirectional.
        
        // For each 'to' gate of pi,
        for (size_t j = 0; j < pi->tos.size(); ++j) {
            bool found = false;

            // Make sure 'to' gate is also connect to pi, i.e. we can find
            // pi from 'from' gate of 'to' gate.
            for (size_t k = 0; k < pi->tos[j]->froms.size(); ++k) { 
                if (pi->tos[j]->froms[k] == pi) {
                    found = true;
                    break;
                }
            }
            
            ASSERT(found,
                << "Gate " << pi->name << " is connected to gate "
                << pi->tos[j]->name << ", but " << pi->tos[j]->name
                << " is not connected to " << pi->name << "\n");
        }
    } // for each pi

    // For each po in primary_outputs,
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        const Cir::Gate* po = cir.primary_outputs[i];

        // Make sure PO has correct type.
        ASSERT(po->module == Cir::Module::PO, 
            << "PO " << po->name << " has type " << po->module << "\n");

        // Make sure PO has name.
        ASSERT(po->name != "", );

        // Make sure PO has 1 input.
        ASSERT(po->froms.size() == 1,
            << "PO " << po->name << " has " << po->froms.size()
            << " input.\n");

        // Make sure PO has 0 output.
        ASSERT(po->tos.size() == 0,
            << "PO " << po->name << " has " << po->tos.size()
            << " input.\n");

        // Make sure all edge is bidirectional.
        
        // For each 'from' gate of po,
        for (size_t j = 0; j < po->froms.size(); ++j) {
            bool found = false;

            // Make sure 'from' gate is also connect to po, i.e. we can
            // find po from 'to' gate of 'from' gate.
            for (size_t k = 0; k < po->froms[j]->tos.size(); ++k) { 
                if (po->froms[j]->tos[k] == po) {
                    found = true;
                    break;
                }
            }
            
            ASSERT(found,
                << "Gate " << po->name << " is connected to gate "
                << po->froms[j]->name << ", but " << po->froms[j]->name
                << " is not connected to" << po->name << "\n");
        }
    } // for each po

    // For each g in logic_gates,
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        const Cir::Gate* g = cir.logic_gates[i];

        // Make sure gate has correct type.
        ASSERT(g->module == Cir::Module::NAND2 ||
               g->module == Cir::Module::NOR2  ||
               g->module == Cir::Module::NOT1    ,
            << "gate " << g->name << " has type " << g->module << "\n");

        // Make sure gate has name.
        ASSERT(g->name != "", );

        // Make sure gate has 1 or more input.
        ASSERT(g->froms.size() > 0,
            << "gate " << g->name << " has " << g->froms.size()
            << " input.\n");

        // Make sure gate has 1 or more output.
        ASSERT(g->tos.size() > 0,
            << "gate " << g->name << " has " << g->tos.size()
            << " input.\n");

        // Make sure all edge is bidirectional.
        
        // For each 'to' gate of g,
        for (size_t j = 0; j < g->tos.size(); ++j) {
            bool found = false;

            // Make sure 'to' gate is also connect to g, i.e. we can
            // find g from 'from' gate of 'to' gate.
            for (size_t k = 0; k < g->tos[j]->froms.size(); ++k) { 
                if (g->tos[j]->froms[k] == g) {
                    found = true;
                    break;
                }
            }
            
            ASSERT(found,
                << "Gate " << g->name << " is connected to gate "
                << g->tos[j]->name << ", but " << g->tos[j]->name
                << " is not connected to " << g->name << "\n");
        }

        // For each 'from' gate of g,
        for (size_t j = 0; j < g->froms.size(); ++j) {
            bool found = false;

            // Make sure 'from' gate is also connect to g, i.e. we can
            // find g from 'to' gate of 'from' gate.
            for (size_t k = 0; k < g->froms[j]->tos.size(); ++k) { 
                if (g->froms[j]->tos[k] == g) {
                    found = true;
                    break;
                }
            }
            
            ASSERT(found,
                << "Gate " << g->name << " is connected to gate "
                << g->froms[j]->name << ", but " << g->froms[j]->name
                << " is not connected to" << g->name << "\n");
        }
    } // for each gate

    std::cerr << __FUNCTION__ << "() passed.\n";
}

// Print circuit state in the format specified by `cir_compare()`.
// Please refer to `cir_compare()`.
//
// #### Input
//
// - cir
// - filename
//
static void print_circuit_state(Sta::Cir::Circuit& cir,
                                const std::string& filename) {
    using namespace Sta;

    std::ofstream fout(filename.c_str());
    ASSERT(fout.good(), << "Cannot open " << filename << "\n");

    fout << "-circuit_name " << cir.name << "\n"
         << "-pi_count " << cir.primary_inputs.size() << "\n"
         << "-po_count " << cir.primary_outputs.size() << "\n"
         << "-gate_count " << cir.logic_gates.size() << "\n"
         << "-nand2_input_pin_name " 
         << cir.modules[Cir::Module::NAND2].input_names[0] << " " 
         << cir.modules[Cir::Module::NAND2].input_names[1] << "\n" 
         << "-nand2_output_pin_name "
         << cir.modules[Cir::Module::NAND2].output_name << "\n"
         << "-nor2_input_pin_name " 
         << cir.modules[Cir::Module::NOR2].input_names[0] << " " 
         << cir.modules[Cir::Module::NOR2].input_names[1] << "\n" 
         << "-nor2_output_pin_name "
         << cir.modules[Cir::Module::NOR2].output_name << "\n"
         << "-not1_input_pin_name " 
         << cir.modules[Cir::Module::NOT1].input_names[0] << "\n"
         << "-not1_output_pin_name "
         << cir.modules[Cir::Module::NOT1].output_name << "\n";

    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        const Cir::Gate* g = cir.primary_outputs[i];
        fout << "-" << g->name << "/PO ";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            fout << g->froms[j]->name << " ";
        }
        fout << "\n";
    }

    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        const Cir::Gate* g = cir.logic_gates[i];
        fout << "-" << g->name << "/"
             << cir.modules[g->module].name << " ";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            fout << cir.modules[g->module].input_names[j] << ":"
                 << g->froms[j]->name << " ";
        }
        fout << "\n";
    }
}

// Test two different cases: case0 and case1.
//
void test_parse(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

    Cir::Circuit cir;
    std::vector<const char*> file_set_A(2);
    file_set_A[0] = "test/cases/case1/input/cadcontest.v";
    file_set_A[1] = "test/cases/case1/input/case1";

    basic_validate(file_set_A, cir);

    cir.clear();
    std::vector<const char*> file_set_B(2);
    file_set_B[0] = "test/cases/case0_module.v";
    file_set_B[1] = "test/cases/case0_netlist.v";

    basic_validate(file_set_B, cir);
    print_circuit_state(cir, "test/cases/case0_state.out");

    cir.clear();
    TestUtil::cir_compare("test/cases/case0_state.ans",
                          "test/cases/case0_state.out");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
