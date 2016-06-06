#include <iostream>
#include <fstream>

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

void test_find_true_paths(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Ana::find_true_paths;

    Record records[6] = {
        RECORD(0, 10, 7),
        RECORD(1, 45, 4),
        RECORD(2, 43, 10),
        RECORD(3, 31, 6),
        RECORD(4, 45, 6),
        RECORD(5, 47, 10)};

    for (int i = 0; i < 6; ++i) {
        Circuit Ckt;

        std::ifstream inf(records[i].cir_file_2);
        ASSERT(inf.good(),
            << "Cannot open '" << records[i].cir_file_2 << "'.\n");

        Sta::Cir::Parser parser(inf, &Ckt);

        ASSERT(parser.parseCase(),
            << "Parse circuit failed while doing case" << i << ".\n");

        std::vector<Path>                paths;
        std::vector< std::vector<bool> > values;
        std::vector<InputVec>            input_vecs;
        
        int return_code;

        return_code = find_true_paths(Ckt,
                                      records[i].time_constraint,
                                      records[i].slack_constraint,
                                      paths, 
                                      values, 
                                      input_vecs);

        ASSERT(return_code == 0, 
            << "Find answer failed while doing case" << i << ".\n");

        std::ofstream outf(records[i].true_path_set_file);
        ASSERT(outf.good(),
            << "Cannot open '" << records[i].true_path_set_file << "'.\n");

        Sta::Cir::Writer writer(outf, &Ckt);

        writer.setConstraint(records[i].time_constraint,
                             records[i].slack_constraint);

        return_code = writer.writeTruePath(paths, 
                                           values, 
                                           input_vecs);

        ASSERT(return_code, 
            << "Write file failed while doing case" << i << ".\n");

        // Clear
        Ckt.clear();

        std::cerr << "Done case" << i << ".\n";
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_find_true_paths_single(void) {
    std::cerr << __FUNCTION__ << "():\n";

    // Create empty circuit.
    Sta::Cir::Circuit Ckt;

    // Open netlist file.
    std::ifstream inf("test/cases/case0/input/case0");

    ASSERT(inf.good(),
        << "Cannot open 'test/cases/case0/input/case0'.\n");

    // Parse netlist file into circuit.
    Sta::Cir::Parser parser(inf, &Ckt);

    ASSERT(parser.parseCase(),
        << "Parse circuit failed while doing case0.\n");

    // find_true_paths() will put its result into these variables.
    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;
    
    int return_code;

    return_code = find_true_paths(Ckt,
                                  10,  // time constraint
                                  7,   // slack constraint
                                  paths, 
                                  values, 
                                  input_vecs);

    ASSERT(return_code == 0, // return_code should be 0 if success.
        << "Find answer failed while doing case0.\n");

    // Open true_path_set_file to write on it.
    std::ofstream outf("test/cases/case0/true_path/case0_true_path_set");

    ASSERT(outf.good(),
        << "Cannot open 'test/cases/case0/true_path/case0_true_path_set'.\n");

    // Create writer to write result.
    Sta::Cir::Writer writer(outf, &Ckt);

    writer.setConstraint(10, 7);

    // Write result.
    return_code = writer.writeTruePath(paths, 
                                       values, 
                                       input_vecs);

    ASSERT(return_code, // return_code == 1 if success.
        << "Write file failed while doing case0");

    // Clear circuit.
    Ckt.clear();

    std::cerr << __FUNCTION__ << "() passed.\n";
}
