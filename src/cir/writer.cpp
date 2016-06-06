#include "sta/src/cir/writer.h"

namespace Sta {
namespace Cir {

using namespace std;

bool Writer::writeTruePath (
	vector< vector<Gate*> > paths,
	vector< vector<bool> > values,
	vector< vector<bool> > input_vecs )
{
	
	vector<string> input_name = _ckt->getInputNames();
	string w41 = "                                                    ";
	string w10 = "          ";
	string line = "";

	_onf
	<< "Header  {  A True Path Set  }  " << endl << endl
	<< "  Benchmark  {  " << _ckt->getCaseName() << "  }" << endl << endl;

	for ( size_t i = 0; i < paths.size(); i++ )
	{
		vector<Gate*> path = paths[i];
		vector<bool> value = values[i];
		vector<bool> input_vec = input_vecs[i];
		int pathDelay = 0;

		_onf
	<< "  Path  {  " << i << "  }" << endl << endl
	<< "  A True Path List" << endl
	<< "  {" << endl
	<< "  ---------------------------------------------------------------------------" << endl
	<< "  Pin    type                            Incr      Path delay" << endl
	<< "  ---------------------------------------------------------------------------" << endl;
		line = w41;
		// input gate
		string input = path.back()->getName() + " (in)";
		line.replace( 0, input.length(), input );
		_onf
	<< "  " << line << "0         0 " << RF( value.back() ) << endl;
		// path gates
		for ( size_t pi = path.size() -1 ; pi > 0; pi-- )
		{
			line = w41;
			string gate = path[pi]->getName() + "/" + getPin( path[pi], path[pi-1] )
				+ " (" + path[pi]->getModel() + ")";
			line.replace( 0, gate.length(), gate );
			_onf
	<< "  " << line << "0" << setw(10) << pathDelay << " " << RF( value[pi] ) << endl;

			line[ line.find("/") + 1 ] = 'Y';
			_onf
	<< "  " << line << "1" << setw(10) << ++pathDelay << " " << RF( value[pi-1] ) << endl;
		}
		// output gate
		line = w41;
		string output = path.front()->getName() + " (out)";
		line.replace( 0, output.length(), output );
		_onf
	<< "  " << line << "0" << setw(10) << pathDelay << " " << RF( value.front() ) << endl;



		_onf
	<< "  ---------------------------------------------------------------------------" << endl
	<< "    Data Required Time" << setw(12) << _dataReqTime << endl
	<< "    Data Arrival Time" << setw(13) << pathDelay << endl
	<< "    ---------------------------------------------------------------------------" << endl
	<< "    Slack" << setw(25) << _slack << endl
	<< "  }" << endl << endl
	<< "  Input Vector" << endl
	<< "  {" << endl;
	
		for ( size_t mi = 0; mi < input_vec.size(); mi++ )
		{
			_onf
	<< "  " << setw(4) << input_name[mi] << "  =  " << input_vec[mi] << endl;
		}	// end of Inputs

		_onf
	<< "  }" << endl << endl;
	}	// end of Path

	_onf
	<< "}" << endl;

	return true;
}

string Writer::getPin ( Gate* from ,Gate* to )
{
	vector<Gate*> inputs = to->getFanIn();
	if ( inputs[0]->getName() == from->getName() ) {
		return "A";
	} else if ( inputs[1]->getName() == from->getName() ) {
		return "B";
	} else return "";
}

} // namespace Cir
} // namespace Sta
