#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

int main ( int argc, char)
{
	Circuit* Ckt = new Circuit();
	ifstream inf;
	inf.open();	// input
	Ckt->parseFile(inf);
	
}