#include <iostream>
#include <iomanip>

#include "sta/src/ana/analyzer.h"
#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/test/src/util/util.h"

static void print_connection(const Sta::Cir::Gate*    g1,
                             const Sta::Cir::Gate*    g2,
                             const Sta::Cir::Circuit& cir) {
    using namespace Sta::Cir;

    std::cout << std::setw(3) << g1->name;

    if (g1->module == Module::NAND2 || 
        g1->module == Module::NOR2  ||
        g1->module == Module::NOT1    ) {

        std::cout << "/" << cir.modules[g1->module].output_name;
    }

    std::cout << " > " << std::setw(3) << g2->name;

    if (g2->module == Module::NAND2 || 
        g2->module == Module::NOR2  ||
        g2->module == Module::NOT1    ) {

        for (size_t i = 0; i < g2->froms.size(); ++i) {
            if (g2->froms[i] == g1) {
                std::cout << "/" << cir.modules[g2->module].input_names[i];
                break;
            }
        }
    }
}

void test_find_sensitizable_paths(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta::Cir;
    using namespace Sta::Ana;

    Circuit cir;

    std::vector<const char*> files;
    files.push_back("test/cases/case0_module.v");
    files.push_back("test/cases/case0_netlist.v");

    int return_code = parse(files, cir);
    ASSERT(return_code == 0, << "Parse into circuit failed.\n");

    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;
    
    return_code = find_sensitizable_paths(
                      cir, 10, 7, paths, values, input_vecs);
    ASSERT(return_code == 0, << "Find answer failed.\n");

    // Print out all paths.
    for (size_t i = 0; i < paths.size(); ++i) {
        //std::cout << "paths[" << i << "]: ";
        
        // Print all gate except PO.
        size_t j = paths[i].size();
        std::cout << "(" << values[i][j - 1] << ") ";
        print_connection(paths[i][j - 1], paths[i][j - 2], cir);

        for (--j; j > 1; --j) {
            std::cout << " > ";
            std::cout << "(" << values[i][j - 1] << ") ";
            print_connection(paths[i][j - 1], paths[i][j - 2], cir);
        }
        std::cout << "\n";
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}


