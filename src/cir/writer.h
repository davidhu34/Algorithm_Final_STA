#ifndef STA_CIR_WRITER_H
#define STA_CIR_WRITER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <stdint.h>
#include <iomanip>

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
		 : _onf(onf), _ckt(ckt)	{ setConstraint( 10, 8 ); }
	void setConstraint ( int dataReqTime, int slack)
	{
		_dataReqTime = dataReqTime;
		_slack = slack;
	}
	bool writeTruePath (
		vector< vector<Gate*> > paths,
		vector< vector<bool> > values,
		vector< vector<bool> > input_vecs );
	string RF ( bool v )	{ return v? "r": "f"; }

private:
	string getPin ( Gate* from, Gate* to);

	int		_dataReqTime;
	int		_slack;
	Circuit*	_ckt;
        ofstream&	_onf;
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_WRITER_H
