#ifndef STA_CIR_CIRCUIT_H
#define STA_CIR_CIRCUIT_H

// start(mine)
#include <vector>
#include <string>

namespace Cir {

struct Module {
    // Type of Module
    enum {
        NAND2,
        NOR2,
        NOT1,
        PI,    // Primary input.
        PO     // Primary output.
    };

    // Data Member
    int                      type;
    std::vector<std::string> input_names;
    std::string              output_name;

    // Constructor
    explicit Module(int _type): type(_type) { }
};

struct Gate {
    // Data Member
    int                module;        // NAND2, NOR2, NOT1, PI or PO
    std::string        name;
    int                value;        // 1, 0 or -1 (floating).
    int                arrival_time; // -1 represent unknown.
    std::vector<Gate*> froms;
    std::vector<Gate*> tos;

    // Constructor
    Gate(int                _module, 
         const std::string& _name   ):
        module      (_module),
        name        (_name),
        value       (-1),
        arrival_time(-1) { }
};

// Path type.
typedef std::vector<const Cir::Gate*> Path;

// Input vector type.
typedef std::vector<bool> InputVec;

struct Circuit {
    // Data Member
    std::vector<Gate*> primary_inputs;
    std::vector<Gate*> primary_outputs;
    std::vector<Gate*> logic_gates;
    
    Module module_NAND2;
    Module module_NOR2;
    Module module_NOT1;

    // Default Constructor
    Circuit(): module_NAND2(Module::NAND2),
               module_NOR2 (Module::NOR2),
               module_NOT1 (Module::NOT1)  { }
};

} // namespace Cir

// end(mine)

/*// start(his)
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "cir/components.h"

namespace Cir {

using std::ifstream;
using std::vector;
using std::map;
using std::string;

typedef std::vector<const Cir::Gate*> Path;
typedef std::vector<bool>             InputVec;

class Circuit
{
public:
    Circuit ()
    {
        case_model["NOT1"] = "not";
        case_model["NAND2"] = "nand";
        case_model["NOR2"] = "nor";
    }
    //void newModel ( string mname, string gtype ) { case_model[mname] = gtype; }
    bool parseFile ( ifstream &inf );

private:
    void connectGates ()
    {
        for ( map< string, Wire*>::iterator wit = _Wire.begin();
            wit != _Wire.end(); wit++) {
            Gate* from = wit->second->getFrom();
            Gate* to = wit->second->getTo();
            from->connectGate( to, "Y");
            to->connectGate( from, wit->second->getToPin() );
        }
    };
    bool inModel ( string str )   
    {
        return ( case_model.find(str) != case_model.end() )?
            true: false;
    };
    string parseWord ( string &parsing );
    vector<string> parseVars ( string &parsing, ifstream &inf, int &line );
    bool parseGate ( string model, string line );
    string trimWire ( string line, string pin );

    void newInput ( string gname )
    { 
        _Inputs.push_back(gname);
        _Gate[gname] = new INPUT(gname);
    };
    void newOutput ( string gname )
    {
        _Outputs.push_back(gname);
        _Gate[gname] = new OUTPUT(gname);
    };
    bool newGate ( string gname, string model, string inA, string inB, string outY )
    {
        string m = case_model[model];
        if ( m == "not" ) {
            if ( inA == "" || outY == "" ) return false;
            else
            {
                _Gate[gname] = new NOT( gname, model );
                wireIn( inA, gname, "A" );
                wireOut( outY, gname );
            }
        } else if ( m == "nand" ) {
            if ( inA == "" || inB == "" || outY == "" ) return false;
            else
            {
                _Gate[gname] = new NAND( gname, model );
                wireIn( inA, gname, "A" );
                wireIn( inB, gname, "B" );
                wireOut( outY, gname );
            }
        } else if ( m == "nor" ) {
            if ( inA == "" || inB == "" || outY == "" ) return false;
            else
            {
                _Gate[gname] = new NOR( gname, model );
                wireIn( inA, gname, "A" );
                wireIn( inB, gname, "B" );
                wireOut( outY, gname );
            }
        } else return false;    // not in model
    };

    void wireOut ( string wname, string gname )
    {
        checkWire(wname);
        _Wire[wname]->setFrom(_Gate[gname]);
    };
    void wireIn ( string wname, string gname, string pin )
    {
        checkWire(wname);
        _Wire[wname]->setTo( _Gate[gname], pin );
    };
    void checkWire ( string wname )
    {
        if ( _Wire.find(wname) == _Wire.end() )
            _Wire[wname] = new Wire(wname);
    };

    vector<string>        _Inputs;
    vector<string>        _Outputs;
    map< string, Wire*>    _Wire;
    map< string, Gate*>    _Gate;

    string                    case_name;
    vector<string>            case_reg;
    map< string, string>     case_model;    // < model, gate type> 
};

} // namespace Cir

// end(his)*/

#endif // STA_CIR_CIRCUIT_H
