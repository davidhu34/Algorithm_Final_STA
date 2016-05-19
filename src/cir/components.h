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
	Wire ( string wname ) { _name = wname; }
	Wire ( bool v ) { _value = v; }
	void setFrom ( Gate* from ) { _from = from; }
	void setTo ( Gate* to ) { _to = to; }
	void setValue ( bool v ) { _value = v; }
	bool getValue () { return _value; }
private:
	string	_name;
	Gate* 	_from;
	Gate* 	_to;
	bool 	_value;
};

class Gate
{
public:
	Gate ( string gname ) { _name = gname; }
	void setWires ( Wire* inA, Wire* inB, Wire* out )
	{
		_wireA = inA;
		_wireB = inB;
		_wireOut.push_back(out);
		/*_inputA = _wireA->getTo();
		_inputB = _wireB->getTo();
		for ( vector<Wire*>::iterator it;
			it != _wireOut.end(); it++ ) {
			_outputs.push_back( it.second->getFrom() );
		}*/
	};
	void outputValue() {}
protected:
	string	 		_name;
	string 			_type;
	Gate* 			_inputA;	// empty for INPUT
	Gate* 			_inputB;	// empty for NOT / INPUT / OUTPUT
	vector<Gate*> 	_outputs;	// empty for OUTPUT
	Wire*			_wireA;		// empty for INPUT
	Wire*			_wireB;		// empty for NOT / INPUT / OUTPUT
	vector<Wire*>	_wireOut;	// empty for OUTPUT
};

class NOT: public Gate
{
public:
	NOT ( string gname ): Gate ( gname ) {}
};
class NAND: public Gate
{
public:
	NAND ( string gname ): Gate ( gname ) {}
};
class NOR: public Gate
{
public:
	NOR ( string gname ): Gate ( gname ) {}
};

class INPUT: public Gate
{
public:
	INPUT ( string gname ): Gate ( gname ) {}
};
class OUTPUT: public Gate
{
public:
	OUTPUT ( string gname ): Gate ( gname ) {}
};

} // namespace Cir

#endif // STA_CIR_COMPONENTS_H
