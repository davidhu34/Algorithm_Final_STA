#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

void testcircuit(void)
{
	Circuit* Ckt = new Circuit();
	ifstream inf;
	inf.open();	// input
	Ckt->parseFile(inf);
	
}
