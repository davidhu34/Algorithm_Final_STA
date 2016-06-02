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
	for ( size_t i =0; i < _Gate.size(); i++ )
	_Gate[i]->printNames();
};
void Circuit::printState ()
{
	std::cout<<"-circuit_name "<<case_name<<"\n"
		     <<"-pi_count "<<_Inputs.size()<<"\n"
			 <<"-po_count "<<_Outputs.size()<<"\n"
			 <<"-gate_count "<<_Gate.size()-
			   _Inputs.size()-_Outputs.size()<<"\n"
			 <<"-nand2_input_pin_name A B\n"
			 <<"-nand2_output_pin_name Y\n"
			 <<"-nor2_input_pin_name A B\n"
			 <<"-nor2_output_pin_name Y\n"
			 <<"-not1_input_pin_name A\n"
			 <<"-not1_output_pin_name Y\n";

	for ( size_t i = 0; i < _Gate.size(); i++ )
		_Gate[i]->printState();

}

void Circuit::newInput ( string gname )
{ 
	_Inputs.push_back(gname);
	_Gate.push_back( new INPUT(gname) );
	_Wire[gname] = new Wire();
	_Wire[gname]->setFrom( _Gate.back() );
};
void Circuit::newOutput ( string gname )
{
	_Outputs.push_back(gname);
	_Gate.push_back( new OUTPUT(gname) );
	_Wire[gname] = new Wire();
	_Wire[gname]->setTo( _Gate.back(), "A" );
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
			_Wire[inA]->setTo( newGate, "A" );
			_Wire[outY]->setFrom(newGate);
		}
	} else if ( m == "nand" ) {
		if ( inA == "" || inB == "" || outY == "" ) return false;
		else
		{
			newGate = new NAND( gname, model );
			_Wire[inA]->setTo( newGate, "A" );
			_Wire[inB]->setTo( newGate, "B" );
			_Wire[outY]->setFrom(newGate);
		}
	} else if ( m == "nor" ) {
		if ( inA == "" || inB == "" || outY == "" ) return false;
		else
		{
			newGate = new NOR( gname, model );
			_Wire[inA]->setTo( newGate, "A" );
			_Wire[inB]->setTo( newGate, "B" );
			_Wire[outY]->setFrom(newGate);
		}
	} else return false;	// not in model

	_Gate.push_back(newGate);
	return true;
};
void Circuit::connectGates ()
{
	for ( map< string, Wire*>::iterator wit = _Wire.begin();
		wit != _Wire.end(); wit++) {
		Gate* from = wit->second->getFrom();
		vector<Gate*> to = wit->second->getTo();
		vector<string> pin = wit->second->getToPin();
		for ( size_t i = 0; i < to.size(); i++ )
		{
			from->connectGate( to[i], "Y" );
			to[i]->connectGate( from, pin[i] );
		}
	}
};
} // namespace Cir
} // namespace Sta
