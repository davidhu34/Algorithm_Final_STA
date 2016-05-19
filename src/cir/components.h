#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

namespace Cir {

using std::string;
using std::vector;

class Wire
{
public:
	Wire () {}
	Wire ( bool v ) { _value = v; }
	void setFrom ( string from ) { _from = from; }
	void setTo ( string to ) { _to = to; }
	void setValue ( bool v ) { _value = v; }
	bool getValue () { return _value; }
private:
	string	name;
	Gate* 	_from;
	Gate* 	_to;
	bool 	_value;
};

class Gate
{
public:
	Gate ( string name ) { _name = name; }
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
private:
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
	NOT(){};
	
};
class NAND: public Gate
{
public:
	NAND () {};
};
class NOR: public Gate
{
public:
	NOR(){};
	
};

class INPUT: Gate
{
public:
	INPUT(){};
};
class OUTPUT: Gate
{
public:
	OUTPUT(){};
};

} // namespace Cir
