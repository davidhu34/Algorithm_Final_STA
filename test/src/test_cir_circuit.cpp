#include <iostream>
#include <fstream>

#include "cir/circuit.h"

using namespace std;

int testcircuit( int argc, char** argv)
{
	Circuit* Ckt = new Circuit();
	ifstream inf;
	inf.open("../input/case1");	// input
	Ckt->parseFile(inf);
}
