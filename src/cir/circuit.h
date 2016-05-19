#include <iostream>
#include <vector>
#include <map>

#include "circuit.cpp"

class Circuit
{
public:
	void newInput ( string gname )
	{ 
		_Inputs.push_back(gname);
		_Gate[gname] = newGate(gname);
	}
	void newOutput ( string gname )
	{
		Outputs.push_back(gname);
		_Gate[gname] = newGate(gname);
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
	
	void checkWire ( string wname )
	{
		if ( _Wire.find(wname) )
			_Wire[wname] = new Wire(wname);
	}
	void wireOut ( string wname, string gmane )
	{
		checkWire(wname);
		_Wire[wname]->setFrom(_Gate[gname]);
	}
	void wireIn ( string wname, string gmane )
	{
		checkWire(wname);
		_Wire[wname]->setTo(_Gate[gname]);
	}
	vector<string> _Inputs;
	vector<string> _Outputs;
	map< string, Wire*> _Wire;
	map< string, Gate*> _Gate;

};