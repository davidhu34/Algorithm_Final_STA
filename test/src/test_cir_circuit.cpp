#include <iostream>
#include <fstream>

#include "cir/circuit.h"

using namespace std;

void test_circuit(void)
{
    std::cerr << __FUNCTION__ << "():\n";

    Cir::Circuit* Ckt = new Cir::Circuit();
    ifstream inf;
    inf.open("../cases/case1/input/case1");	// input
    Ckt->parseFile(inf);

    std::cerr << __FUNCTION__ << "() passed.\n";
}
