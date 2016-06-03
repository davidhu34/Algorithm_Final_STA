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
using namespace std;
// Forward declaration.
class Wire;
class Gate;

class Wire
{
public:
	Wire () {}
	void setFrom ( Gate* from )		{ _from = from; }
	void setTo ( Gate* to, string pin )	{ _to.push_back(to); _pin.push_back(pin); }
	Gate* getFrom ()	{ return _from; }
	vector<Gate*> getTo ()		{ return _to; }
	vector<string> getToPin ()	{ return _pin; }

private:
	Gate* 	_from;
	vector<Gate*>	_to;
	vector<string>	_pin;
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
	
	virtual void printNames () = 0;
	virtual void printState () const = 0;

	string getName ()		{ return _name; }
	string getModel ()		{ return _model; }
	vector<Gate*> getFanIn ()				{ return _inputs; }
	vector<Gate*> getFanOut ()				{ return _outputs; }

	bool		_value;
	int        		_tag;
	int        		_var;
	unsigned	_arrivalTime;

protected:
	string	 		_name;
	string 			_model;
	vector<Gate*>		_inputs;
	vector<Gate*>		_outputs;	// empty for OUTPUT
	friend class Circuit;
};

class NOT: public Gate
{
public:
	NOT ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames ()
	{
		cout<<_name<<":  "<<"A: "<<_inputs[0]->getName()<<", Y: ";
		for ( size_t i = 0; i < _outputs.size(); i++ ) cout<<_outputs[i]->getName();
		cout<<endl;
	}
	void printState () const
	{
		// Print fanin
		cout<<"-"<<_name<<"/"<<_model<<"/fanin A:"
			<<_inputs[0]->getName()<<"\n";

		// Print fanout
		cout<<"-"<<_name<<"/"<<_model<<"/fanout ";
		for ( size_t i = 0; i < _outputs.size(); i++ )
			cout<<"Y:"<<_outputs[i]->getName()<<" ";

		cout<<"\n";
	}
};
class NAND: public Gate
{
public:
	NAND ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "B") {
			_inputs[1] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames ()
	{
		cout<<_name<<":  "<<"A: "<<_inputs[0]->getName()<<", B: "<<_inputs[1]->getName()<<", Y: ";
		for ( size_t i = 0; i < _outputs.size(); i++ ) cout<<_outputs[i]->getName();
		cout<<endl;
	}
	void printState () const
	{
		// Print fanin
		cout<<"-"<<_name<<"/"<<_model<<"/fanin A:"<<_inputs[0]->getName()
			<<" B:"<<_inputs[1]->getName()<<"\n";

		// Print fanout
		cout<<"-"<<_name<<"/"<<_model<<"/fanout ";
		for ( size_t i = 0; i < _outputs.size(); i++ )
			cout<<"Y:"<<_outputs[i]->getName()<<" ";

		cout<<"\n";
	}
};
class NOR: public Gate
{
public:
	NOR ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "B") {
			_inputs[1] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames ()
	{
		cout<<_name<<":  "<<"A: "<<_inputs[0]->getName()<<", B: "<<_inputs[1]->getName()<<", Y: ";
		for ( size_t i = 0; i < _outputs.size(); i++ ) cout<<_outputs[i]->getName();
		cout<<endl;
	}
	void printState () const
	{
		// Print fanin
		cout<<"-"<<_name<<"/"<<_model<<"/fanin A:"<<_inputs[0]->getName()
			<<" B:"<<_inputs[1]->getName()<<"\n";

		// Print fanout
		cout<<"-"<<_name<<"/"<<_model<<"/fanout ";
		for ( size_t i = 0; i < _outputs.size(); i++ )
			cout<<"Y:"<<_outputs[i]->getName()<<" ";

		cout<<"\n";
	}
};

class INPUT: public Gate
{
public:
	INPUT ( string gname ): Gate ( gname ) {}
	void connectGate ( Gate* gate, string pin )
	{
cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "Y" )  _outputs.push_back(gate); }
	void printNames ()
	{
		cout<<_name<<":  "<<" Y: ";
		for ( size_t i = 0; i < _outputs.size(); i++ ) cout<<_outputs[i]->getName();
		cout<<endl;
	}
	void printState () const
	{
		cout<<"-"<<_name<<"/PI/fanout ";
		for ( size_t i = 0; i < _outputs.size(); i++ )
			cout<<_outputs[i]->getName()<<" ";

		cout<<"\n";
	}
};
class OUTPUT: public Gate
{
public:
	OUTPUT ( string gname ): Gate ( gname )	{ _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" )  _inputs[0] = gate; }
	
	void printNames ()
	{
		cout<<_name<<":  "<<"A: "<<_inputs[0]->getName()<<endl;
	}
	void printState () const
	{
		cout<<"-"<<_name<<"/PO/fanin "<<_inputs[0]->getName()<<"\n";
	}
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_COMPONENTS_H
