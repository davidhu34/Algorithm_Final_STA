#include <iostream>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/ana/analyzer.h"

#include "sta/test/src/util/util.h"

void test_write(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta::Cir;
    using namespace Sta::Ana;

    Circuit cir;

    std::vector<const char*> files;
    files.push_back("test/cases/case0/input/cadcontest.v");
    files.push_back("test/cases/case0/input/case0");

    int return_code = parse(files, cir);
    ASSERT(return_code == 0, << "Parse into circuit failed.\n");

    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;
    
    return_code = find_true_paths(
                      cir, 10, 7, paths, values, input_vecs);
    ASSERT(return_code == 0, << "Find answer failed.\n");

    write(cir, 10, 7, paths, values, input_vecs, 
          "test/cases/case0/true_path/case0_true_path_set");

    // clear
    cir.clear();

    std::cerr << __FUNCTION__ << "() passed.\n";
}
