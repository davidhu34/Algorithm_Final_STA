#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "sta/src/cir/components.h"
#include "sta/src/cir/circuit.h"

using namespace std;

static bool moduleERR ( int line )
{ 
	cerr << "invalid module file format (line "
		<< line << ")."<<endl;
	return false;
}
static bool regERR ( int line )
{
	cerr << "name not registerred in module (line "
		<< line << ")."<<endl;
	return false;
}

namespace Sta {
namespace Cir {

bool Circuit::parseFile ( ifstream &inf )
{   // premise: first line starts with "module <case_name>"
	int parsing_line = 1;   // track error
	string parsing_str;
	string type = "";   // input / output / modle name 

	getline( inf, parsing_str );
	cout<<parsing_str<<endl;
	type = parseWord( inf, parsing_str );
	if ( type != "module" )
		return moduleERR(parsing_line); 
	else    // module initialized
	{   // get case name
		cout<<parsing_str<<endl;
		type = parseWord( inf, parsing_str );
		if ( type == "" )   return moduleERR(parsing_line);
		case_name = type;
		if ( parseVars( parsing_str, inf, parsing_line).size() == 0 )
			moduleERR(parsing_line);
	}

	while (1)
	{
		type = parseWord( inf, parsing_str );
		if ( type == "" ) return moduleERR(parsing_line);
		
		if ( inModel(type) ) {
			if ( !parseGate( inf, type, parsing_str ) )
				return moduleERR(parsing_line);
			else parsing_line++;
		} else if ( type == "endmodule") break;
		else
		{
			vector<string> var_tmp = parseVars( parsing_str, inf, parsing_line );
			
			if ( var_tmp.empty() ) moduleERR(parsing_line);
			if ( type == "input" ) {
				for ( size_t i = 0; i < var_tmp.size(); i++ )	newInput( var_tmp[i] );
			} else if ( type == "output" ) {
				for ( size_t i = 0; i < var_tmp.size(); i++ )	newOutput( var_tmp[i] );
			} else if ( type == "wire" ) {
				for ( size_t i = 0; i < var_tmp.size(); i++ )	newWire( var_tmp[i] );
			} else return moduleERR(parsing_line);		// invalid type 
		}
	}
	connectGates();
	return true;
}

string Circuit::parseWord ( ifstream &inf, string &parsing )
{
	string word = "";
	while ( parsing == "" )	getline( inf, parsing );
	while ( isspace( parsing[0] ) )	parsing.erase(0,1);
	while ( isalnum( parsing[0] ) )
	{
		word += parsing[0];
		parsing.erase(0,1);
	}
	cout<< "word: " << word << endl;
	return word;    // empty word for ERR
}

vector<string> Circuit::parseVars ( string &parsing, ifstream &inf, int &line )
{
	vector<string> vars;
	string parsed_tmp;
	bool flag = true;	// false flag: module ERR

	cout<<"vars: ";
	while (flag)
	{
		if ( isalnum( parsing[0] ) )
			parsed_tmp += parsing[0];
		else if ( parsing[0] == ',' || parsing[0] == ';' )
		{
			vars.push_back(parsed_tmp);
			//cout<<parsed_tmp<<parsing[0]<<" ";
			parsed_tmp = "";
			if ( parsing[0] == ';')
			{
				if ( !getline( inf, parsing ) )
					flag = false;
				else line++;
				break;
			}
		}// else if ( parsed_tmp != "" && isspace( parsing[0] ) )	flag = false;

		if ( parsing.length() > 1 ) parsing.erase(0,1);
		else if ( getline( inf, parsing ) ) line++;
		else flag = false;
	}   // empty vector for ERR
	cout<<endl;
	return (flag)? vars: vector<string>();
}

bool Circuit::parseGate ( ifstream &inf, string model, string &parsing )
{
	string gname, inputA, inputB, outputY;
	gname = parseWord( inf, parsing );
	if ( gname == "" ) return false;
	inputA = trimWire( parsing, "A");
	inputB = trimWire( parsing, "B");
	outputY = trimWire( parsing, "Y");
	cout<<"Y:" <<outputY<<endl;
	return newLogicGate( gname, model, inputA, inputB, outputY ) && getline( inf, parsing );
}

string Circuit::trimWire ( string line, string pin )
{
	size_t pStart, pEnd;
	pStart = line.find( "." + pin +"(" );
	if ( pStart == string::npos ) return "";
	else pEnd = line.find( ")", pStart + 3);
	if ( pEnd == string::npos ) return "";
	return line.substr( pStart + 3, pEnd - pStart - 3 );
}

} // namespace Cir
} // namespace Sta
