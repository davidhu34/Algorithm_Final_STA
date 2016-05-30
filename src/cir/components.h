#ifndef STA_CIR_COMPONENTS_H
#define STA_CIR_COMPONENTS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

namespace Sta {
namespace Cir {

using std::string;
using std::vector;

// Forward declaration.
class Wire;
class Gate;

class Wire
{
public:
	Wire ( string wname ) { _name = wname; _pin = "A"; }
	void setFrom ( Gate* from ) { _from = from; }
	void setTo ( Gate* to, string pin ) { _to = to; _pin = pin; }
	Gate* getFrom () { return _from; }
	Gate* getTo () { return _to; }
	string getToPin () { return _pin; }

private:
	string	_name;
	Gate* 	_from;
	Gate* 	_to;
	string	_pin;
};

class Gate
{
public:
	Gate ( string gname , string mname = "IO" )
	{
		_name = gname;
		_model = mname;
	}
	virtual void connectGate ( Gate* gate, string pin ) = 0;

protected:
	string	 		_name;
	string 			_model;
	Gate* 			_inputA;	// empty for INPUT
	Gate* 			_inputB;	// empty for NOT / INPUT / OUTPUT
	vector<Gate*>		_outputs;	// empty for OUTPUT
};

class NOT: public Gate
{
public:
	NOT ( string gname, string mname ): Gate ( gname, mname ) {};
	void connectGate ( Gate* gate, string pin )
	{
		if ( pin == "A" ) {
			_inputA = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
};
class NAND: public Gate
{
public:
	NAND ( string gname, string mname ): Gate ( gname, mname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		if ( pin == "A" ) {
			_inputA = gate;
		} else if ( pin == "B") {
			_inputB = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
};
class NOR: public Gate
{
public:
	NOR ( string gname, string mname ): Gate ( gname, mname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		if ( pin == "A" ) {
			_inputA = gate;
		} else if ( pin == "B") {
			_inputB = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
};

class INPUT: public Gate
{
public:
	INPUT ( string gname ): Gate ( gname ) {}
	void connectGate ( Gate* gate, string pin )
	{	if ( pin == "Y" )  _outputs.push_back(gate); }
};
class OUTPUT: public Gate
{
public:
	OUTPUT ( string gname ): Gate ( gname ) {}
	void connectGate ( Gate* gate, string pin )
	{	if ( pin == "A" )  _inputA = gate; }
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_COMPONENTS_H
