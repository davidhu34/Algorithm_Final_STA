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
class Writer
{
public:
	Writer ( ofstream &onf, Circuit* ckt )
		 : _onf(onf), _ckt(ckt) {}
	bool writeTruePath ();

private:
	
	Circuit*		_ckt;
	ofstream&		_onf;
};

} // namespace Sta
} // namespace Cir

#endif // STA_CIR_PARSER_H
