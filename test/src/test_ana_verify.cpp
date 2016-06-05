#include <iostream>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/ana/analyzer.h"

#include "sta/test/src/util/util.h"

void test_verify_true_path_set(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::parse;
    using Sta::Cir::parse_true_path_set;
    using Sta::Ana::find_true_paths;
    using Sta::Ana::verify_true_path_set;

    Circuit cir;

    std::vector<const char*> files;
    files.push_back("test/cases/case0_module.v");
    files.push_back("test/cases/case0_netlist.v");

    int return_code = parse(files, cir);
    ASSERT(return_code == 0, << "Parse into circuit failed.\n");

    int                              time_constraint  = 10;
    int                              slack_constraint = 7;
    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;
    
    return_code = find_true_paths(cir, 
                                  time_constraint,
                                  slack_constraint,
                                  paths, 
                                  values, 
                                  input_vecs);
    ASSERT(return_code == 0, << "Find answer failed.\n");

    const char* true_path_set_file = "test/cases/case0_true_path_set";

    return_code = write(cir, 
                        time_constraint,
                        slack_constraint,
                        paths, 
                        values, 
                        input_vecs,
                        true_path_set_file);
    ASSERT(return_code == 0, << "Write failed.\n");

    paths.clear();
    values.clear();
    input_vecs.clear();

    return_code = parse_true_path_set(true_path_set_file,
                                      cir,
                                      time_constraint,
                                      slack_constraint,
                                      paths,
                                      values,
                                      input_vecs);
    ASSERT(return_code == 0, << "Parse true path set failed.\n");

    return_code = verify_true_path_set(cir,
                                       time_constraint,
                                       slack_constraint,
                                       paths,
                                       values,
                                       input_vecs);
    ASSERT(return_code == 0, << "Verification process failed.\n");

    // Clear
    cir.clear();

    std::cerr << __FUNCTION__ << "() passed.\n";
}
