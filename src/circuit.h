#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

#include "components.h"

class Circuit
{
public:
	Circuit ()
	{
		_riseWire = new Wire(true);
		_fallWire = new Wire(false);
	}
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
	bool parseFile ( ifstream &inf ) {}

private:
	bool inModel ( string str ) {}
	string parseWord ( string &parsing ) {}
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
	string				case_name;
	vector<string>		case_reg;

	Wire*		_riseWire;
	Wire* 		_fallWire;

};
