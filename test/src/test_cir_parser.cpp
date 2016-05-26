#include <iostream>

#include "test/src/util/util.h"

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
static void test_parse_indep(const std::vector<const char*>& files,
                             Cir::Circuit& cir) {

    std::cerr << __FUNCTION__ << "():\n";

    int ret = Cir::parse(files, cir);
    ASSERT(!ret, << "Cir::parse() return " << ret << "\n");

    // Make sure all modules has correct number of input and
    // output names.
    
    ASSERT(cir.module_NAND2.input_names.size() == 2,
        << "Module NAND2 has " << cir.module_NAND2.input_names.size()
        << " name.\n");

    ASSERT(cir.module_NOR2.input_names.size() == 2,
        << "Module NOR2 has " << cir.module_NOR2.input_names.size()
        << " name.\n");

    ASSERT(cir.module_NOT1.input_names.size() == 1,
        << "Module NOT1 has " << cir.module_NOT1.input_names.size()
        << " name.\n");

    ASSERT(cir.module_NAND2.output_name != "", );
    ASSERT(cir.module_NOR2.output_name != "", );
    ASSERT(cir.module_NOT1.output_name != "", );

    // Make sure circuit has name.
    ASSERT(cir.name != "", );

    // For each pi in primary_inputs,
    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        const Cir::Gate* pi = &cir.primary_inputs[i];

        // Make sure PI has correct type.
        ASSERT(pi->type == Cir::Module::PI, 
            << "PI " << pi->name << " has type " << pi->type << "\n");

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
        const Cir::Gate* po = &cir.primary_outputs[i];

        // Make sure PO has correct type.
        ASSERT(po->type == Cir::Module::PO, 
            << "PO " << po->name << " has type " << po->type << "\n");

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

            // Make sure 'from' gate is also connect to po, i.e. we can find
            // po from 'to' gate of 'from' gate.
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
        const Cir::Gate* g = &cir.logic_gates[i];

        // Make sure gate has correct type.
        ASSERT(g->type == Cir::Module::NAND2 ||
               g->type == Cir::Module::NOR2  ||
               g->type == Cir::Module::NOT1    ,
            << "gate " << g->name << " has type " << g->type << "\n");

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

            // Make sure 'to' gate is also connect to g, i.e. we can find
            // g from 'from' gate of 'to' gate.
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

            // Make sure 'from' gate is also connect to g, i.e. we can find
            // g from 'to' gate of 'from' gate.
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

// Do content dependent test, i.e. answer will be different depends
// on circuit.
//
// - It has correct number of gates:
//     - Number of primary inputs is correct.
//     - Number of primary outputs is correct.
//     - Number of logic gates is correct.
// - Name of netlist and I/O pins of modules is correct.
// - All gate has correct connection.
//
// #### Input
//
// - `cir`: Parsed circuit.
//
static void test_parse_dep(Cir::Circuit& cir) {
    std::cerr << __FUNCTION__ << "():\n";

    // Make sure it has correct number of gates.

    ASSERT(cir.primary_inputs.size() == 13,
        << "It has " << cir.primary_inputs.size() << " PI.\n");

    ASSERT(cir.primary_outputs.size() == 13,
        << "It has " << cir.primary_outputs.size() << " PO.\n");

    ASSERT(cir.logic_gates.size() == 13,
        << "It has " << cir.logic_gates.size() << " Logic Gates.\n");

    // Make sure name of netlist and I/O pins of modules is correct.

    ASSERT(cir.name == "case0",
        << "Its name is " << cir.name << "\n");

    ASSERT(cir.module_NAND2.input_names[0] == "A",
        << "Its name is " << cir.module_NAND2.input_names[0] << "\n");

    ASSERT(cir.module_NAND2.input_names[1] == "B",
        << "Its name is " << cir.module_NAND2.input_names[1] << "\n");

    ASSERT(cir.module_NAND2.output_name == "Y",
        << "Its name is " << cir.module_NAND2.output_name << "\n");
        
    ASSERT(cir.module_NOR2.input_names[0] == "A",
        << "Its name is " << cir.module_NOR2.input_names[0] << "\n");

    ASSERT(cir.module_NOR2.input_names[1] == "B",
        << "Its name is " << cir.module_NOR2.input_names[1] << "\n");

    ASSERT(cir.module_NOR2.output_name == "Y",
        << "Its name is " << cir.module_NOR2.output_name << "\n");
        
    ASSERT(cir.module_NOT1.input_names[0] == "A",
        << "Its name is " << cir.module_NOT1.input_names[0] << "\n");

    ASSERT(cir.module_NOT1.output_name == "Y",
        << "Its name is " << cir.module_NOT1.output_name << "\n");

    // Make sure all gate has correct connection.
    
    const char* outfile = "test/cases/check_cir.out";
    const char* ansfile = "test/cases/check_cir.ans";

    // Print gates (including PO and PI) to outfile with this format:
    //
    // ```
    // >U1  A:U13 B:U30
    // >U84 B:U52 A:U67 B:U98
    // ...
    // ```
    // 
    // Token start with '>' is a gate. All token following it are fanin
    // of it. `>U1 A:U13 B:U30` means U1 input from U13 through pin A,
    // input from U30 through pin B.
    //
    std::ofstream fout(outfile);
    ASSERT(fout.good(), << "Cannot open file \"" << outfile << "\".\n");

    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        const Gate* g = cir.primary_outputs[i];
        fout << ">" << g->name << " ";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            fout << cir.modules[g->type].input_names[j] << ":"
                 << g->froms[j]->name << " ";
        }
        fout << "\n";
    }

    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        const Gate* g = cir.logic_gates[i];
        fout << ">" << g->name << " ";

        for (size_t j = 0; j < g->froms.size(); ++j) {
            fout << cir.modules[g->type].input_names[j] << ":"
                 << g->froms[j]->name << " ";
        }
        fout << "\n";
    }

    typedef std::string                       Str
    typedef std::set<Str>                     StrSet;
    typedef std::set<Str>::iterator           SetIter;
    typedef std::map<Str, StrSet>::value_type Pair;
    typedef std::map<Str, StrSet>::iterator   MapIter;

    // Map gate name to a set of gates that it is connected to.
    std::map<Str, StrSet> gate_map;
    Str                   token;
    Str                   gate_name;

    // Read in answer file and put them into gate_map, so that we
    // can check whether there is a difference between ansfile and
    // outfile.
    //
    std::ifstream fin(ansfile);
    ASSERT(fin.good(), << "Cannot open file \"" << ansfile << "\".\n");

    while (fin >> token) {
        if (token[0] == '>') {
            gate_name = token.substr(1);

            std::pair<MapIter, bool> p = 
                gate_map.insert((Pair(gate_name, StrSet())));

            ASSERT(p.second,
                << "Gate " << p.first->first << " existed.\n");
        }
        else {
            std::pair<SetIter, bool> p = 
                gate_map[gate_name].insert(token);

            ASSERT(p.second,
                << "Gate " << *p.first << " is already pointed by "
                << gate_name << ".\n");
        }
    }
    fin.close();
    fin.clear();

    // Read in gate output file. For each gate, check whether it is
    // inside gate_map. For each out fanout of a gate, check whether
    // it is inside gate_map[gate].
    //
    fin.open(outfile);
    ASSERT(fin.good(), << "Cannot open file \"" << outfile << "\".\n");

    while (fin >> token) {
        if (token[0] == '>') {
            gate_name = token.substr(1);

            MapIter it = gate_map.find(gate_name);

            ASSERT(it != gate_map.end(),
                << "Couldn't find gate " << gate_name << ".\n");
        }
        else {
            SetIter it = gate_map[gate_name].find(token);

            ASSERT(it != gate_map[gate_name].end(),
                << "Gate " << gate_name << " is not pointing to "
                << token << ".\n");
        }
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}

// Test two different cases: case0 and case1.
//
void test_parse(void) {
    Cir::cir;
    std::vector<const char*> file_set_A(2);
    file_set_A[0] = "test/cases/case1/input/cadcontest.v";
    file_set_A[1] = "test/cases/case1/input/case1";

    test_parse_indep(file_set_A, cir);
    test_parse_dep(cir);

    Cir::clear_circuit(cir);
    std::vector<const char*> file_set_B(2);
    file_set_B[0] = "test/cases/case0_module.v";
    file_set_B[1] = "test/cases/case0_netlist.v";

    test_parse_indep(file_set_B, cir);
    test_parse_dep(cir);
}
