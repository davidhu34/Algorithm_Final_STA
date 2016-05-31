#include <iostream>
#include <fstream>

#include "sta/src/cir/circuit.h"

using namespace std;

void test_circuit(void)
{
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

    Cir::Circuit* Ckt = new Cir::Circuit();
    ifstream inf ("test/cases/case0_netlist_nocomment.v", ifstream::in);
    if ( Ckt->parseFile(inf) )
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

    Cir::Circuit* Ckt = new Cir::Circuit();
    ifstream inf ("test/cases/case0_netlist_nocomment.v", ifstream::in);
    if ( Ckt->parseFile(inf) )
    {
        Ckt->printState();
    }

    std::cerr << __FUNCTION__ << "() passed.\n";
}
