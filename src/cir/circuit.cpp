#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "sta/src/cir/components.h"
#include "sta/src/cir/circuit.h"

using namespace std;

namespace Sta {
namespace Cir {

void Circuit::testPrint () const
{
	for ( size_t i =0; i < _LogicGates.size(); i++ )
	_LogicGates[i]->printNames();
};
void Circuit::printState () const
{
	std::cout<<"-circuit_name "<<case_name<<"\n"
		     <<"-pi_count "<<_Inputs.size()<<"\n"
			 <<"-po_count "<<_Outputs.size()<<"\n"
			 <<"-gate_count "<<_LogicGates.size()<<"\n"
			 <<"-nand2_input_pin_name A B\n"
			 <<"-nand2_output_pin_name Y\n"
			 <<"-nor2_input_pin_name A B\n"
			 <<"-nor2_output_pin_name Y\n"
			 <<"-not1_input_pin_name A\n"
			 <<"-not1_output_pin_name Y\n";

	for ( size_t i = 0; i < _LogicGates.size(); i++ )
		_LogicGates[i]->printState();

    typedef map<string, Gate*>::const_iterator Iter;
	for ( Iter it = _Inputs.begin(); it != _Inputs.end(); ++it )
		it->second->printState();

	for ( size_t i = 0; i < _Outputs.size(); i++ )
		_Outputs[i]->printState();

}

void Circuit::clear() {
	typedef std::map<std::string, Wire*>::iterator IterW;

	for (IterW it = _Wires.begin(); it != _Wires.end(); ++it) {
		delete it->second;
	}
	_Wires.clear();

	typedef std::map<std::string, Gate*>::iterator IterG;
	
	for (IterG it = _Inputs.begin(); it != _Inputs.end(); ++it) {
		delete it->second;
	}
	_Inputs.clear();

	for (size_t i = 0; i < _Outputs.size(); ++i) {
		delete _Outputs[i];
	}
	_Outputs.clear();

	for (size_t i = 0; i < _LogicGates.size(); ++i) {
		delete _LogicGates[i];
	}
	_LogicGates.clear();
}

void Circuit::newInput ( string gname )
{ 
	_Inputs[gname] = new INPUT(gname);
};
void Circuit::newOutput ( string gname )
{	
	OUTPUT* output = new OUTPUT(gname);
	_Outputs.push_back(output);
	_Wires[gname] = new Wire();
	_Wires[gname]->setTo( output, "A" );
};
bool Circuit::newLogicGate ( string gname, string model, string inA, string inB, string outY )
{
	string m = case_models[model];
	Gate* newGate;
//cout<< "new gate: "<<endl;
	if ( m == "not" ) {
		if ( inA == "" || outY == "" ) return false;
		else
		{
			newGate = new NOT( gname, model );
			linkWire( newGate, inA, "A" );
			linkWire( newGate, outY, "Y" );
		}
	} else if ( m == "nand" ) {
		if ( inA == "" || inB == "" || outY == "" ) return false;
		else
		{
			newGate = new NAND( gname, model );
			linkWire( newGate, inA, "A" );
			linkWire( newGate, inB, "B" );
			linkWire( newGate, outY, "Y" );
		}
	} else if ( m == "nor" ) {
		if ( inA == "" || inB == "" || outY == "" ) return false;
		else
		{
			newGate = new NOR( gname, model );
			linkWire( newGate, inA, "A" );
			linkWire( newGate, inB, "B" );
			linkWire( newGate, outY, "Y" );
		}
	} else return false;	// not in model

	_LogicGates.push_back(newGate);
	return true;
};

void Circuit::linkWire ( Gate* g, string wname, string pin )
{
	if ( isInput(wname) ) {
		g->connectGate( _Inputs[wname], pin );
		_Inputs[wname]->connectGate( g, "Y" );
	} else if ( pin == "Y" ) 	_Wires[wname]->setFrom(g);
		else		_Wires[wname]->setTo( g, pin );
}

void Circuit::connectGates ()
{
	for ( map< string, Wire*>::iterator wit = _Wires.begin();
		wit != _Wires.end(); wit++) {
		Gate* from = wit->second->getFrom();
		vector<Gate*> to = wit->second->getTo();
		vector<string> pin = wit->second->getToPin();
		for ( size_t i = 0; i < to.size(); i++)
		{
			from->connectGate( to[i], "Y" );
			to[i]->connectGate( from, pin[i] );
		}
		
	}
};

vector<string> Circuit::getInputNames () const
{
	vector<string> names;
	for ( map< string, Gate*>::const_iterator it = _Inputs.begin();
		it != _Inputs.end(); it++) 
		names.push_back( it->first );
	return names;
};

vector< vector<Gate*> > truepathBruteForce ( vector<bool> input_vec )
{
	// assue input_vec matches input
	vector<Gate*> pending;
	for ( map< string, Gate*>::iterator iit = _Inputs.begin();
		iit != _Inputs.end(); iit++ ) {
		pending.push_back( iit->second );
	}
	while ( !pendin.empty() )
	{
		vector<Gate*> pendingNew;
		for ( size_t i = 0; i < pending.size(); i++ )
		{
			if ( pending[i]->checkOutputValue() ) {
				vector<Gate> outs = pending[i]->getFanOut();
			} else {
				pendingNew.push_back( pending[i] )
			}
		}
	}
};
} // namespace Cir
} // namespace Sta
