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

typedef std::vector<const Cir::Gate*> Path;
typedef std::vector<bool>             InputVec;

class Circuit
{
public:
	Circuit ()
	{
		_riseWire = new Wire(true);
		_fallWire = new Wire(false);
		case_model["NOT1"] = "not";
		case_model["NAND2"] = "nand";
		case_model["NOR2"] = "nor";
	}
	//void newModel ( string mname, string gtype ) { case_model[mname] = gtype; }
	bool parseFile ( ifstream &inf );

private:
	void connectGates ()
	{
		for ( map<string, Gate*>::iterator it = _Gate.begin();
			it != _Gate.end(); it++ ) {
			it->second->
		}
	};
	bool inModel ( string str )
	{
		return ( case_model.find(str) != case_model.end() )?
			true: false;
	}
	string parseWord ( string &parsing );
	vector<string> parseVars ( string &parsing, ifstream &inf, int &line );
	bool parseGate ( string model, string line );
	string trimWire ( string line, string pin );

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
	bool newGate ( string gname, string model, string inA, string inB, string outY )
	{
		switch ( case_model[model] )
		{
			case "not":
				if ( inA == "" || outY == "" ) return false();
				else
				{
					_Gate[gname] = new NOT( gname, model );
					wireIn( inA, gname, "A" );
					wireOut( outY, gname );
				}	break;
			case "nand":
				if ( inA == "" || inB == "" || outY == "" ) return false();
				else
				{
					_Gate[gname] = new NAND( gname, model );
					wireIn( inA, gname, "A" );
					wireIn( inB, gname, "B" );
					wireOut( outY, gname );
				}	break;
			case "nor":
				if ( inA == "" || inB == "" || outY == "" ) return false();
				else
				{
					_Gate[gname] = new NOR( gname, model );
					wireIn( inA, gname, "A" );
					wireIn( inB, gname, "B" );
					wireOut( outY, gname );
				}	break;
			default:	// not in model
				return false;
				break;
		}
	}

	void wireOut ( string wname, string gname )
	{
		checkWire(wname);
		_Wire[wname]->setFrom(_Gate[gname]);
	}
	void wireIn ( string wname, string gname, string pin )
	{
		checkWire(wname);
		_Wire[wname]->setTo( _Gate[gname], pin );
	}
	void checkWire ( string wname )
	{
		if ( _Wire.find(wname) == _Wire.end() )
			_Wire[wname] = new Wire(wname);
	}

	vector<string>		_Inputs;
	vector<string>		_Outputs;
	map< string, Wire*>	_Wire;
	map< string, Gate*>	_Gate;

	string					case_name;
	vector<string>			case_reg;
	map< string, string> 	case_model;	// < model, gate type> 

	Wire*		_riseWire;
	Wire* 		_fallWire;

};

} // namespace Cir

#endif // STA_CIR_CIRCUIT_H
