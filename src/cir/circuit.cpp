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

void Circuit::testPrint () 
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

    typedef map<string, Gate*>::iterator Iter;
	for ( Iter it = _Inputs.begin(); it != _Inputs.end(); ++it )
		it->second->printState();

	for ( size_t i = 0; i < _Outputs.size(); i++ )
		_Outputs[i]->printState();

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
cout<< "new gate: "<<endl;
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
		Gate* to = wit->second->getTo();
		string pin = wit->second->getToPin();
		from->connectGate( to, "Y" );
		to->connectGate( from, pin );
	}
};
} // namespace Cir
} // namespace Sta
