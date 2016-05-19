#include <iostream>
#include <fstream>
#include <vector>
#include <map>

//#include "cir/Circuit.h"

using namespace std;

void testcircuit(void)
{
	Circuit* Ckt = new Circuit();
	ifstream inf;
	inf.open();	// input
	Ckt->parseFile(inf);
}
