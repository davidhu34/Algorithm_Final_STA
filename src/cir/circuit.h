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
	Circuit ()
	{
		case_model["NOT1"] = "not";
		case_model["NAND2"] = "nand";
		case_model["NOR2"] = "nor";
	}
	void setCaseName ( string cname )	{ case_name = cname; }
	void newWire ( string wname )		{ _Wire[wname] = new Wire(); }
	
	void testPrint ();
	void printState ();
	
	void newInput ( string gname );
	void newOutput ( string gname );
	bool newLogicGate ( string gname, string model, string inA, string inB, string outY );
	
	void connectGates ();

private:
	vector<string>		_Inputs;
	vector<string>		_Outputs;
	map< string, Wire*>	_Wire;
	vector<Gate*>	_Gate;

	string					case_name;
	vector<string>			case_reg;
	map< string, string> 	case_model;	// < model, gate type> 
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_CIRCUIT_H
