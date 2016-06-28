#include <assert.h>
#include <iostream>

#include "sta/src/cir/subcircuit.h"
#include "sta/src/cir/compare.h"
#include "sta/src/cir/writer.h"
#include "sta/src/cir/parser.h"

#define DUMP_FILE(str) "test/cases/case0/output/" str ".dump"
#define ANS_FILE(str)  DUMP_FILE(str) ".ans"

void test_subcircuit(void) {
    using Sta::Cir::Circuit;
    using Sta::Cir::Subcircuit;
    using Sta::Cir::dump;
    using Sta::Cir::compare_dump;

    Circuit cir;

    const char* dump_file[4] = {
        DUMP_FILE("m3"), DUMP_FILE("m2"), 
        DUMP_FILE("m1"), DUMP_FILE("m0") };

    const char* ans_file[4] = {
        ANS_FILE("m3"), ANS_FILE("m2"), 
        ANS_FILE("m1"), ANS_FILE("m0") };

    std::vector<const char*> files;
    files.push_back("test/cases/case0/input/cadcontest.v");
    files.push_back("test/cases/case0/input/case0");

    assert(parse(files, cir));

    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Subcircuit subcir(cir, cir.primary_outputs[i]);
        dump(subcir, dump_file[i]);
        
        assert(compare_dump(ans_file[i], dump_file[i]));
    }
}
