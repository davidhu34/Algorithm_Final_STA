#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/cir/compare.h"
#include "sta/test/src/util/util.h"

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

    using namespace Sta::Cir;

    ASSERT(parse(files, cir), << "parse() failed.\n");

    // Make sure all modules has correct number of input and
    // output names.
    
    const Module* mod = &cir.modules[Module::NAND2];
    ASSERT(mod->input_names.size() == 2,
        << "Module NAND2 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    mod = &cir.modules[Module::NOR2];
    ASSERT(mod->input_names.size() == 2,
        << "Module NOR2 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    mod = &cir.modules[Module::NOT1];
    ASSERT(mod->input_names.size() == 1,
        << "Module NOT1 has " << mod->input_names.size() << " name.\n");
    ASSERT(mod->output_name != "", );

    // Make sure circuit has name.
    ASSERT(cir.name != "", );

    // For each pi in primary_inputs,
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        const Gate* pi = cir.primary_inputs[i];

        // Make sure PI has correct type.
        ASSERT(pi->module == Module::PI, 
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
        const Gate* po = cir.primary_outputs[i];

        // Make sure PO has correct type.
        ASSERT(po->module == Module::PO, 
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
        const Gate* g = cir.logic_gates[i];

        // Make sure gate has correct type.
        ASSERT(g->module == Module::NAND2 ||
               g->module == Module::NOR2  ||
               g->module == Module::NOT1    ,
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

// Test two different cases: case0 and case1.
//
void test_parse(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta::Cir;
    using Sta::Cir::compare_dump;
    using Sta::Cir::dump;

    const char* case_num[10] = {
        "test/cases/case1/input/cadcontest.v",
        "test/cases/case1/input/case1",
        "test/cases/case2/input/cadcontest.v",
        "test/cases/case2/input/case2",
        "test/cases/case3/input/cadcontest.v",
        "test/cases/case3/input/case3",
        "test/cases/case4/input/cadcontest.v",
        "test/cases/case4/input/case4",
        "test/cases/case5/input/cadcontest.v",
        "test/cases/case5/input/case5"};

    for (int i = 0; i < 10; i += 2) {
        Circuit cir;
        std::cout << "Validating " << case_num[i + 1] << "...\n";

        std::vector<const char*> file_set(2);
        file_set[0] = case_num[i];
        file_set[1] = case_num[i + 1];

        basic_validate(file_set, cir);
    }

    std::vector<const char*> file_set_B(2);
    file_set_B[0] = "test/cases/case0/input/cadcontest.v";
    file_set_B[1] = "test/cases/case0/input/case0";

    std::cout << "Validating " << file_set_B[1] << "...\n";

    Circuit cir;
    basic_validate(file_set_B, cir);

    const char* filename = "test/cases/case0/output/case0.dump";
    ASSERT(dump(cir, filename), << "Dump failed.\n");

    compare_dump("test/cases/case0/output/case0.dump.ans", filename);

    std::cerr << __FUNCTION__ << "() passed.\n";
}
