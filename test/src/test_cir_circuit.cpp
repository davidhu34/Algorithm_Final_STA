#include <iostream>
#include <fstream>

#include "cir/circuit.h"

using namespace std;

int test_circuit( int argc, char** argv)
{
    Cir::Circuit* Ckt = new Cir::Circuit();
    ifstream inf;
    inf.open("../cases/case1/input/case1");	// input
    Ckt->parseFile(inf);

    return 0;
}
