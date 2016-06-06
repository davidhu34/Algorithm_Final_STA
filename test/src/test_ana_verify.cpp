#include <iostream>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/ana/analyzer.h"

#include "sta/test/src/util/util.h"

struct Record {
    const char* cir_file_1;
    const char* cir_file_2;
    const char* true_path_set_file;
    int         time_constraint;
    int         slack_constraint;

    Record(const char* _cir_file_1,
           const char* _cir_file_2,
           const char* _true_path_set_file,
           int         _time_constraint,
           int         _slack_constraint):
        cir_file_1        (_cir_file_1        ),
        cir_file_2        (_cir_file_2        ),
        true_path_set_file(_true_path_set_file),
        time_constraint   (_time_constraint   ),
        slack_constraint  (_slack_constraint  )  { }
};

#define RECORD(n, time_constraint, slack_constraint)                    \
    Record("test/cases/case" #n "/input/cadcontest.v",                  \
           "test/cases/case" #n "/input/case" #n,                       \
           "test/cases/case" #n "/true_path/case" #n "_true_path_set",  \
           time_constraint,                                             \
           slack_constraint)

void test_verify_true_path_set(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::parse;
    using Sta::Cir::parse_true_path_set;
    using Sta::Ana::find_true_paths;
    using Sta::Ana::verify_true_path_set;

    Record records[5] = {
        RECORD(1, 45, 4),
        RECORD(2, 43, 10),
        RECORD(3, 31, 6),
        RECORD(4, 45, 6),
        RECORD(5, 47, 10)};

    for (int i = 0; i < 5; ++i) {
        Circuit cir;

        int return_code = parse(records[i].cir_file_1,
                                records[i].cir_file_2,
                                cir);

        ASSERT(return_code == 0,
            << "Parse circuit failed while doing case" << i << ".\n");

        std::vector<Path>                paths;
        std::vector< std::vector<bool> > values;
        std::vector<InputVec>            input_vecs;
        
        return_code = find_true_paths(cir, 
                                      records[i].time_constraint,
                                      records[i].slack_constraint,
                                      paths, 
                                      values, 
                                      input_vecs);

        ASSERT(return_code == 0, 
            << "Find answer failed while doing case" << i << ".\n");

        return_code = write(cir, 
                            records[i].time_constraint,
                            records[i].slack_constraint,
                            paths, 
                            values, 
                            input_vecs,
                            records[i].true_path_set_file);

        ASSERT(return_code == 0, 
            << "Write file failed while doing case" << i << ".\n");

        paths.clear();
        values.clear();
        input_vecs.clear();

        return_code = parse_true_path_set(records[i].true_path_set_file,
                                          cir,
                                          records[i].time_constraint,
                                          records[i].slack_constraint,
                                          paths,
                                          values,
                                          input_vecs);

        ASSERT(return_code == 0, 
            << "Parse true path set file failed while doing case"
            << i << ".\n");

        return_code = verify_true_path_set(cir,
                                           records[i].time_constraint,
                                           records[i].slack_constraint,
                                           paths,
                                           values,
                                           input_vecs);

        ASSERT(return_code == 0,
            << "Verification failed while doing case" << i << ".\n");

        // Clear
        cir.clear();

        std::cerr << "Done case" << i + 1 << ".\n";
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}