#include <iostream>
#include <fstream>

#include "circuit.h"

using namespace std;

int main ( int argc, char** argv)
{
	Circuit* Ckt = new Circuit();
	ifstream inf;
	inf.open("../input/case1");	// input
	Ckt->parseFile(inf);
}