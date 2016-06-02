#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "sta/src/cir/components.h"

namespace Sta {
namespace Cir {

using std::ifstream;
using std::vector;
using std::map;
using std::string;

typedef std::vector<const Cir::Gate*> Path;
typedef std::vector<bool>             InputVec;

class Circuit
{
public:
	Circuit () {}
	void setModels ( map< string, string> models )		{ case_models = models; }
	map<string, string> getModels () 	{ return case_models; }
	void setCaseName ( string cname )	{ case_name = cname; }
	void newWire ( string wname )		{ _Wires[wname] = new Wire(); }
	void newInput ( string gname );
	void newOutput ( string gname );
	bool newLogicGate ( string gname, string model, string inA, string inB, string outY );
	
	void connectGates ();
	
	void testPrint ();
	void printState ();
	

private:
	void linkWire ( Gate* g, string wname, string pin );
	bool isInput ( string wname )	{ return _Inputs.find(wname) != _Inputs.end(); }

	map< string, Wire*>	_Wires;		// no wire to inputs
	map< string, Gate*>	_Inputs;
	vector<Gate*>		_Outputs;
	vector<Gate*>		_LogicGates;

	string			case_name;
	map< string, string> 	case_models;	// < model, gate type> 
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_CIRCUIT_H
