#ifndef STA_CIR_COMPONENTS_H
#define STA_CIR_COMPONENTS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

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
		switch (pin)
		{
			case "A":
				_inputA = gate; break;
			case "Y":
				_outputs.push_back(gate); break;
			default:
				break;
		}
	}
};
class NAND: public Gate
{
public:
	NAND ( string gname, string mname ): Gate ( gname, mname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		switch (pin)
		{
			case "A":
				_inputA = gate; break;
			case "B":
				_inputB = gate; break;
			case "Y":
				_outputs.push_back(gate); break;
			default:
				break;
		}
	}
};
class NOR: public Gate
{
public:
	NOR ( string gname, string mname ): Gate ( gname, mname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		switch (pin)
		{
			case "A":
				_inputA = gate; break;
			case "B":
				_inputB = gate; break;
			case "Y":
				_outputs.push_back(gate); break;
			default:
				break;
		}
	}
};

class INPUT: public Gate
{
public:
	INPUT ( string gname ): Gate ( gname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		switch (pin)
		{
			case "Y":
				_outputs.push_back(gate); break;
			default:
				break;
		}
	}
};
class OUTPUT: public Gate
{
public:
	OUTPUT ( string gname ): Gate ( gname ) {}
	void connectGate ( Gate* gate, string pin )
	{
		switch (pin)
		{
			case "A":
				_inputA = gate; break;
			default:
				break;
		}
	}
};

} // namespace Cir

#endif // STA_CIR_COMPONENTS_H
