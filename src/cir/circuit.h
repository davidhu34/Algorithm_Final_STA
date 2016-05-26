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
		case_model["NOT1"] = "not";
		case_model["NAND2"] = "nand";
		case_model["NOR2"] = "nor";
	}
	//void newModel ( string mname, string gtype ) { case_model[mname] = gtype; }
	bool parseFile ( ifstream &inf );

private:
	void connectGates ()
	{
		for ( map< string, Wire*>::iterator wit = _Wire.begin();
			wit != _Wire.end(); wit++) {
			Gate* from = wit->second->getFrom();
			Gate* to = wit->second->getTo();
			from->connectGate( to, "Y");
			to->connectGate( from, wit->second->getToPin() );
		}
	};
	bool inModel ( string str )   
	{
		return ( case_model.find(str) != case_model.end() )?
			true: false;
	};
	string parseWord ( string &parsing );
	vector<string> parseVars ( string &parsing, ifstream &inf, int &line );
	bool parseGate ( ifstream &inf, string model, string parsing );
	string trimWire ( string line, string pin );

	void newWire ( string wname )
	{
		_Wire[wname] = new Wire();
	}
	void newInput ( string gname )
	{ 
		_Inputs.push_back(gname);
		_Gate.push_back( new INPUT(gname) );
	};
	void newOutput ( string gname )
	{
		_Outputs.push_back(gname);
		_Gate.push_back( new OUTPUT(gname) );
	};
	bool newGate ( string gname, string model, string inA, string inB, string outY )
	{
		string m = case_model[model];
		Gate* newGate;

		if ( m == "not" ) {
			if ( inA == "" || outY == "" ) return false;
			else
			{
				newGate = new NOT( gname, model );
				_Wire[inA]->setTo( newGate, "A" );
				_Wire[outY]->setFrom(newGate);
			}
		} else if ( m == "nand" ) {
			if ( inA == "" || inB == "" || outY == "" ) return false;
			else
			{
				newGate = new NAND( gname, model );
				_Wire[inA]->setTo( newGate, "A" );
				_Wire[inB]->setTo( newGate, "B" );
				_Wire[outY]->setFrom(newGate);
			}
		} else if ( m == "nor" ) {
			if ( inA == "" || inB == "" || outY == "" ) return false;
			else
			{
				newGate = new NOR( gname, model );
				_Wire[inA]->setTo( newGate, "A" );
				_Wire[inB]->setTo( newGate, "B" );
				_Wire[outY]->setFrom(newGate);
			}
		} else return false;	// not in model

		_Gate.push_back(newGate);
		return true;
	};

	vector<string>		_Inputs;
	vector<string>		_Outputs;
	map< string, Wire*>	_Wire;
	vector<Gate*>	_Gate;

	string					case_name;
	vector<string>			case_reg;
	map< string, string> 	case_model;	// < model, gate type> 
};

} // namespace Cir

#endif // STA_CIR_CIRCUIT_H
