#include <iostream>
#include <fstream>

#include "sta/src/cir/compare.h"
#include "sta/src/cir/parser.h"

using namespace std;

void test_circuit(void)
{
    std::cerr << __FUNCTION__ << "():\n";
    using namespace Sta;

    Cir::Circuit* Ckt = new Cir::Circuit();
    ifstream inf ("test/cases/case0_netlist.v", ifstream::in);
    Cir::Parser* parser = new Cir::Parser( inf, Ckt );

    if ( parser->parseCase() )
    {
        cout<<"success"<<endl;
        Ckt->testPrint();
    }
    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_circuit_state(void)
{
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

    Cir::Circuit Ckt;
    ifstream inf ("test/cases/case0_netlist_nocomment.v", ifstream::in);
    Cir::Parser parser(inf, &Ckt);
    if ( parser.parseCase() )
    {
        // Trying to redirect all things to cout to fout.

        // Redirect output to this file.
        std::ofstream fout("test/cases/case0_state.out");

        // Save cout's stream buffer.
        std::streambuf* coutbuf = std::cout.rdbuf();

        // Make cout's bufferptr point to buffer of fout.
        std::cout.rdbuf(fout.rdbuf());

        Ckt.printState();

        // Restore cout's stream buffer.
        std::cout.rdbuf(coutbuf);
    }

    Sta::Cir::compare_dump("test/cases/case0_state.ans",
                           "test/cases/case0_state.out");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
