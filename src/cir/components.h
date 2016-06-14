#ifndef STA_CIR_COMPONENTS_H
#define STA_CIR_COMPONENTS_H

#include <stdint.h>
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
	virtual ~Gate() { }

	virtual void connectGate ( Gate* gate, string pin ) = 0;
	
	virtual void printNames () const = 0;
	virtual void printState () const = 0;

	virtual bool checkArrival () = 0;
	virtual void checkTruePath () = 0;
	virtual void bfReset () = 0;
	bool getBfOutput ()	{ return _bfOutput; }
	int getBfDelay ()	{ return _bfDelay; }
	void setBFInput ( bool v ) { _bfOutput = v; }

	string getName () const  { return _name; }
	string getModel () const { return _model; }
	vector<Gate*>& getFanIn () { return _inputs; }
	vector<Gate*>& getFanOut () { return _outputs; }

	uint8_t     _value;
	int        	_tag;
	int        	_var;
	unsigned	_arrivalTime;

protected:
	string	 		_name;
	string 			_model;
	vector<Gate*>		_inputs;
	vector<Gate*>		_outputs;	// empty for OUTPUT

	int _bfDelay;
	bool _bfOutput;

	friend class Circuit;
};

class NOT: public Gate
{
public:
	NOT ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
//cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames () const
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

	void bfReset ()
	{
		_bfDelay = -1;
		_bfOutput = false;
		_bfIsTP = false;
		_bfPrevDelay = -1;
	}
	bool checkArrival ()
	{
		if ( _bfPrevDelay == -1 )
			_bfPrevDelay = _inputs[0]->getBfDelay();
		return _bfPrevDelay != -1;
	}
	void checkTruePath ()
	{
		_bfIsTP = true;
		_bfOutput = !( _inputs[0]->getBfOutput() );
		_bfDelay = _bfPrevDelay + 1;
	}

private:
	bool _bfIsTP;
	int _bfPrevDelay;
};
class NAND: public Gate
{
public:
	NAND ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
//cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "B") {
			_inputs[1] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames () const
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

	void bfReset ()
	{
		_bfDelay = -1;
		_bfOutput = false;
		for ( int i = 0; i < 2 ; i++ ) {
			_bfIsTP[i] = false;
			_bfPrevDelay[i] = -1;
		}
	}
	bool checkArrival ()
	{
		for ( int i = 0; i < 2; i++ )
			if ( _bfPrevDelay[i] == -1 )
				_bfPrevDelay[i] = _inputs[i]->getBfDelay();
		return _bfPrevDelay[0] != -1 && _bfPrevDelay[1] != -1;
	}
	void checkTruePath ()
	{
		int delay1 = _bfPrevDelay[0];
		int delay2 = _bfPrevDelay[1];
		bool V1 = _inputs[0]->getBfOutput();
		bool V2 = _inputs[1]->getBfOutput();
		if ( delay1 == delay2 ) {
			if ( V1 && V2 ) {
				_bfOutput = false;
				_bfIsTP[0] = true;
				_bfIsTP[1] = true;
			} else {
				_bfOutput = true;
				if (!V1) _bfIsTP[1] = true;
				if (!V2) _bfIsTP[0] = true;
			}
		} else if ( delay1 > delay2 ) {
			if ( V1 && V2 ) {
				_bfOutput = false;
				_bfIsTP[0] = true;
			} else {
				_bfOutput = true;
				if (!V2) _bfIsTP[1] = true;
				else _bfIsTP[0] = true;
			}
		} else if ( delay2 > delay1 ) {
			if ( V1 && V2 ) {
				_bfOutput = false;
				_bfIsTP[1] = true;
			} else {
				_bfOutput = true;
				if (!V1) _bfIsTP[0] = true;
				else _bfIsTP[1] = true;
			}
		}
	}

private:
	bool _bfIsTP[2];
	int _bfPrevDelay[2];
};

class NOR: public Gate
{
public:
	NOR ( string gname, string mname ): Gate ( gname, mname )
	{	_inputs.push_back(0); _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
//cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" ) {
			_inputs[0] = gate;
		} else if ( pin == "B") {
			_inputs[1] = gate;
		} else if ( pin == "Y" ) {
			_outputs.push_back(gate);
		} else return;
	}
	void printNames () const
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

	void bfReset ()
	{
		_bfDelay = -1;
		_bfOutput = false;
		for ( int i = 0; i < 2 ; i++ ) {
			_bfIsTP[i] = false;
			_bfPrevDelay[i] = -1;
		}
	}
	bool checkArrival ()
	{
		for ( int i = 0; i < 2; i++ )
			if ( _bfPrevDelay[i] == -1 )
				_bfPrevDelay[i] = _inputs[i]->getBfDelay();
		return _bfPrevDelay[0] != -1 && _bfPrevDelay[1] != -1;
	}
	void checkTruePath ()
	{
		int delay1 = _bfPrevDelay[0];
		int delay2 = _bfPrevDelay[1];
		bool V1 = _inputs[0]->getBfOutput();
		bool V2 = _inputs[1]->getBfOutput();
		if ( delay1 == delay2 ) {
			if ( !V1 && !V2 ) {
				_bfOutput = true;
				_bfIsTP[0] = true;
				_bfIsTP[1] = true;
			} else {
				_bfOutput = false;
				if (V1) _bfIsTP[1] = true;
				if (V2) _bfIsTP[0] = true;
			}
		} else if ( delay1 > delay2 ) {
			if ( !V1 && !V2 ) {
				_bfOutput = true;
				_bfIsTP[0] = true;
			} else {
				_bfOutput = false;
				if (V2) _bfIsTP[1] = true;
				else _bfIsTP[0] = true;
			}
		} else if ( delay2 > delay1 ) {
			if ( !V1 && !V2 ) {
				_bfOutput = true;
				_bfIsTP[1] = true;
			} else {
				_bfOutput = false;
				if (V1) _bfIsTP[0] = true;
				else _bfIsTP[1] = true;
			}
		}
	}

private:
	bool _bfIsTP[2];
	int _bfPrevDelay[2];
};

class INPUT: public Gate
{
public:
	INPUT ( string gname ): Gate ( gname, "PI" ) {}
	void connectGate ( Gate* gate, string pin )
	{
//cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "Y" )  _outputs.push_back(gate); }
	void printNames () const
	{
		cout<<_name<<":  "<<" Y: ";
		for ( size_t i = 0; i < _outputs.size(); i++ ) cout<<_outputs[i]->getName();
		cout<<endl;
	}
	void printState () const
	{
		cout<<"-"<<_name<<"/"<<_model<<"/fanout ";
		for ( size_t i = 0; i < _outputs.size(); i++ )
			cout<<_outputs[i]->getName()<<" ";

		cout<<"\n";
	}

	void bfReset ()
	{
		_bfDelay = -1;
		_bfOutput = false;
	}
	bool checkArrival () { return true; }
	void checkTruePath ()
	{
		_bfDelay = 0;
	}
};
class OUTPUT: public Gate
{
public:
	OUTPUT ( string gname ): Gate ( gname, "PO" )	{ _inputs.push_back(0); }
	void connectGate ( Gate* gate, string pin )
	{
//cout<<"connect "<< pin << " of "<< _name << ": " << gate->getName() << endl;
		if ( pin == "A" )  _inputs[0] = gate; }
	
	void printNames () const
	{
		cout<<_name<<":  "<<"A: "<<_inputs[0]->getName()<<endl;
	}
	void printState () const
	{
		cout<<"-"<<_name<<"/"<<_model<<"/fanin "<<_inputs[0]->getName()<<"\n";
	}

	void bfReset ()
	{
		_bfDelay = -1;
		_bfOutput = false;
		_bfIsTP = false;
		_bfPrevDelay = -1;
	}
	bool checkArrival ()
	{
		if ( _bfPrevDelay == -1 )
			_bfPrevDelay = _inputs[0]->getBfDelay();
		return _bfPrevDelay != -1;
	}
	void checkTruePath ()
	{
		_bfIsTP = true;
		_bfOutput = _inputs[0]->getBfOutput();
		_bfDelay = _bfPrevDelay + 1;
	}

private:
	bool _bfIsTP;
	int _bfPrevDelay;
};

} // namespace Cir
} // namespace Sta

#endif // STA_CIR_COMPONENTS_H
