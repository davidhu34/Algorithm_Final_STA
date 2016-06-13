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

    ASSERT(parse(files, cir),
        << "Parse into circuit failed.\n");

    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;
    
    ASSERT(find_true_paths(cir, 10, 7, paths, values, input_vecs),
        << "Find answer failed.\n");

    ASSERT(write(cir, 10, 7, paths, values, input_vecs, 
                 "test/cases/case0/true_path/case0_true_path_set"),
        << "Write failed.\n");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
