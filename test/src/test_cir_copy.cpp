#include <cassert>
#include <iostream>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"

namespace {

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

} // namespace

#define RECORD(n, time_constraint, slack_constraint)                    \
    Record("test/cases/case" #n "/input/cadcontest.v",                  \
           "test/cases/case" #n "/input/case" #n,                       \
           "test/cases/case" #n "/true_path/case" #n "_true_path_set",  \
           "test/cases/case" #n "/output/case" #n ".dump",              \
           "test/cases/case" #n "/output/case" #n ".dump.ans",          \
           time_constraint,                                             \
           slack_constraint)

void test_circuit_copy(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using Sta::Cir::Circuit;
    using Sta::Cir::Gate;
    using Sta::Cir::parse;

    Record records[6] = {
        RECORD(0, 10, 7),
        RECORD(1, 45, 4),
        RECORD(2, 43, 10),
        RECORD(3, 31, 6),
        RECORD(4, 45, 6),
        RECORD(5, 47, 10)};

    for (int i = 0; i < 6; ++i) {
        Circuit cir;

        int return_code = parse(records[i].cir_file_1,
                                records[i].cir_file_2,
                                cir);

        assert(return_code == 0);

        Circuit cir2(cir);

    #define COMPARE_GATE(group)                                      \
        assert(cir.group.size() == cir2.group.size());               \
                                                                     \
        for (size_t i = 0; i < cir.group.size(); ++i) {              \
            Gate* g1 = cir.group[i];                                 \
            Gate* g2 = cir2.group[i];                                \
                                                                     \
            assert(g1                   != g2                  );    \
            assert(g1->module           == g2->module          );    \
            assert(g1->value            == g2->value           );    \
            assert(g1->tag              == g2->tag             );    \
            assert(g1->arrival_time     == g2->arrival_time    );    \
            assert(g1->min_arrival_time == g2->min_arrival_time);    \
            assert(g1->var              == g2->var             );    \
            assert(g1->name             == g2->name            );    \
            assert(g1->froms.size()     == g2->froms.size()    );    \
            assert(g1->tos.size()       == g2->tos.size()      );    \
                                                                     \
            for (size_t j = 0; j < g1->froms.size(); ++j) {          \
                assert(g1->froms[j]       != g2->froms[j]      );    \
                assert(g1->froms[j]->name == g2->froms[j]->name);    \
            }                                                        \
            for (size_t j = 0; j < g1->tos.size(); ++j) {            \
                assert(g1->tos[j]       != g2->tos[j]      );    \
                assert(g1->tos[j]->name == g2->tos[j]->name);        \
            }                                                        \
        }
                                                                     
        COMPARE_GATE(primary_inputs)
        COMPARE_GATE(primary_outputs)
        COMPARE_GATE(logic_gates)
    }

    std::cerr << __FUNCTION__ << "() passed.\n";                     
}                                                                    
                                                                     
