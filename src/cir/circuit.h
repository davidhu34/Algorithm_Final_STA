#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "cir/components.h"

namespace Cir {

using std::ifstream;
using std::vector;
using std::map;
using std::string;

class Circuit
{
public:
	Circuit ()
	{
		_riseWire = new Wire(true);
		_fallWire = new Wire(false);
	}
	void newModel ( string mname, string gtype )
	{ 	case_model[mname] = gtype; }	// TODO
	void newInput ( string gname )
	{ 
		_Inputs.push_back(gname);
		_Gate[gname] = new INPUT(gname);
	}
	void newOutput ( string gname )
	{
		_Outputs.push_back(gname);
		_Gate[gname] = new OUTPUT(gname);
	}
	void newWire ( string wname ) { _Wire[wname] = new Wire(wname); }
	void newGate ( string line )
	{
		string gateName;
		vector<string> wireName;

		_Gate[gateName] = new Gate(gateName);
		wireOut( wireName.back(), gateName );
		wireIn( wireName[0], gateName );
		if ( wireName.size() == 3 )
			wireIn( wireName[1], gateName );
	}
	bool parseFile ( ifstream &inf );

private:
	bool inModel ( string str )
	{
		return ( case_model.find(str) != case_model.end() )?
			true: false;
	}
	string parseWord ( string &parsing );
	vector<string> parseVars ( string &parsing, ifstream &inf, int &line );
	void checkWire ( string wname )
	{
		if ( _Wire.find(wname) != _Wire.end() )
			_Wire[wname] = new Wire(wname);
	}
	void wireOut ( string wname, string gname )
	{
		checkWire(wname);
		_Wire[wname]->setFrom(_Gate[gname]);
	}
	void wireIn ( string wname, string gname )
	{
		checkWire(wname);
		_Wire[wname]->setTo(_Gate[gname]);
	}

	vector<string>		_Inputs;
	vector<string>		_Outputs;
	map< string, Wire*> _Wire;
	map< string, Gate*>	_Gate;

	string					case_name;
	vector<string>			case_reg;
	map< string, string> 	case_model;	// < model, gate type> 

	Wire*		_riseWire;
	Wire* 		_fallWire;

};

} // namespace Cir

#endif // STA_CIR_CIRCUIT_H
