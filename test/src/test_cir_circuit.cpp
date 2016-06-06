#include <iostream>
#include <fstream>

#include "sta/src/cir/compare.h"
#include "sta/src/cir/parser.h"

#include "sta/test/src/util/util.h"

using namespace std;

void test_circuit(void)
{
    std::cerr << __FUNCTION__ << "():\n";
    using namespace Sta;

    Cir::Circuit Ckt;
    ifstream inf ("test/cases/case0/input/case0", ifstream::in);
    Cir::Parser parser( inf, &Ckt );

    if ( parser.parseCase() )
    {
        cout<<"success"<<endl;
        Ckt.testPrint();
    }

    Ckt.clear();

    std::cerr << __FUNCTION__ << "() passed.\n";
}

struct Record {
    const char* cir_file_1;
    const char* cir_file_2;
    const char* true_path_set_file;
    const char* dump_file;
    const char* dump_ans_file;
    int         time_constraint;
    int         slack_constraint;

    Record(const char* _cir_file_1,
           const char* _cir_file_2,
           const char* _true_path_set_file,
           const char* _dump_file,
           const char* _dump_ans_file,
           int         _time_constraint,
           int         _slack_constraint):

        cir_file_1        (_cir_file_1        ),
        cir_file_2        (_cir_file_2        ),
        true_path_set_file(_true_path_set_file),
        dump_file         (_dump_file         ),
        dump_ans_file     (_dump_ans_file     ),
        time_constraint   (_time_constraint   ),
        slack_constraint  (_slack_constraint  )  { }
};

#define RECORD(n, time_constraint, slack_constraint)                    \
    Record("test/cases/case" #n "/input/cadcontest.v",                  \
           "test/cases/case" #n "/input/case" #n,                       \
           "test/cases/case" #n "/true_path/case" #n "_true_path_set",  \
           "test/cases/case" #n "/output/case" #n ".dump",              \
           "test/cases/case" #n "/output/case" #n ".dump.ans",          \
           time_constraint,                                             \
           slack_constraint)

void test_circuit_state(void)
{
    std::cerr << __FUNCTION__ << "():\n";

    Record records[6] = {
        RECORD(0, 10, 7),
        RECORD(1, 45, 4),
        RECORD(2, 43, 10),
        RECORD(3, 31, 6),
        RECORD(4, 45, 6),
        RECORD(5, 47, 10)};

    for (size_t i = 0; i < 6; ++i) {
        Sta::Cir::Circuit Ckt;

        ifstream inf(records[i].cir_file_2);
        ASSERT(inf.good(),
            << "Cannot open '" << records[i].cir_file_2 << "'.\n");

        Sta::Cir::Parser parser(inf, &Ckt);
        if ( parser.parseCase() )
        {
            // Trying to redirect all things to cout to fout.

            // Redirect output to this file.
            std::ofstream fout(records[i].dump_file);
            ASSERT(fout.good(),
                << "Cannot open '" << records[i].dump_file << "'\n");

            // Save cout's stream buffer.
            std::streambuf* coutbuf = std::cout.rdbuf();

            // Make cout's bufferptr point to buffer of fout.
            std::cout.rdbuf(fout.rdbuf());

            Ckt.printState();

            // Restore cout's stream buffer.
            std::cout.rdbuf(coutbuf);
        }

        Ckt.clear();

        int return_code = Sta::Cir::compare_dump(
                              records[i].dump_file,
                              records[i].dump_ans_file);

        ASSERT(return_code == 0,
            << "Compare dump failed for case" << i << ".\n");
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}
