#ifndef STA_CIR_PARSER_H
#define STA_CIR_PARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "sta/src/cir/components.h"
#include "sta/src/cir/circuit.h"

namespace Sta {
namespace Cir {

using namespace std;
// put function prototype here.
class Parser
{
public:
	Parser ( ifstream &inf, Circuit* ckt )
	{
		_inf = inf;
		_ckt = ckt;
		parseModel();
	}
	bool parseModel ()
	{
		_models["NOT1"] = "not";
		_models["NAND2"] = "nand";
		_models["NOR2"] = "nor";
		return true;
	}
	bool parseCase ();
private:
	bool inModel ( string str )
	{
		return ( _models.find(str) != _models.end() )?
			true: false;
	}
	bool moduleERR ();
	string parseWord ();
	vector<string> parseVars ();
	bool parseGate ( string model );
	string trimWire ( string pin );
	bool getNextLine ();

	Circuit*		_ckt;
	map< string, string> 	_models;
	ifstream		_inf;
	string			_parsingStr;
	int			_parsingLine;

};


} // namespace Sta
} // namespace Cir

#endif // STA_CIR_PARSER_H
