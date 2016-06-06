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
	Parser ( ifstream &inf, Circuit* ckt ): _inf(inf)
	{
		_ckt = ckt;
		_parsingLine = 0;
		_parsingStr = "";
		parseModel();
	}
	bool parseModel ()
	{
		map< string, string> models;
		models["NOT1"] = "not";
		models["NAND2"] = "nand";
		models["NOR2"] = "nor";
		_ckt->setModels(models);
		return true;
	}
	bool parseCase ();

private:
	bool moduleERR ();
	string parseWord ();
	vector<string> parseVars ();
	bool parseGate ( string model );
	string trimWire ( string pin );
	bool getNextLine ();

	Circuit*		_ckt;
	ifstream&		_inf;
	string			_parsingStr;
	int			_parsingLine;
};

int parse_true_path_set(
    const std::string&                true_path_set_file,
    const Circuit&                    cir,
    int                               time_constraint,
    int                               slack_constraint,
    std::vector<Path>&                paths,
    std::vector< std::vector<bool> >& values,
    std::vector<InputVec>&            input_vecs);

} // namespace Sta
} // namespace Cir

#endif // STA_CIR_PARSER_H
