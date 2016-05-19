#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

bool moduleERR ( int line )
{ 
	cerr << "invalid module file format (line "
		<< line << ")."<<endl;
	return false;
}
bool regERR ( int line )
{
	cerr << "name not registerred in module (line "
		<< line << ")."<<endl;
	return false;
}

void Circuit::parseFile ( ifstream &inf )
{	// premise: first line starts with "module <case_name>"
	int parsing_line = 1;	// track error
	string parsing_str;
	type = "";	// input / output / modle name 

	getline( inf, parsing_str );
	type = parseWord(parsing_str);
	if ( type != "module" )
		return moduleERR(parsing_line); 
	else	// module initialized
	{	// get case name
		type = parseWord(parsing_str);
		if ( type == "" )	return moduleERR(parsing_line);
		parseVars( parsing_str, inf );	// register names
	}

	while (1)
	{
		type = parseWord(parsing_str);
		if ( type == "") return moduleERR(line);
		if ( inModel(type) )
			if ( !parseGate( parsing_str ) )
				return moduleERR(parsing_line);
		else if ( type == "endmodule") break;
		{
			vector<string> var_tmp = parseVars( parsing_str, inf );
			if ( var_tmp.empty() ) moduleERR(parsing_line);
			switch (type)
			{
				case "input":
					for( std::vector<string>::iterator it;
						it != var_emp.end(); it++ ) {
						newInput(it.second);
					}	break;
				case "output":
					for( std::vector<string>::iterator it;
						it != var_emp.end(); it++ ) {
						newOutput(it.second);
					}	break;
				case "wire":
					for( std::vector<string>::iterator it;
						it != var_emp.end(); it++ ) {
						newWire(it.second);
					}	break;
				default:	// invalid type	
					return moduleERR(line)
			}	
		}
	}
	return true;
}


string parseWord ( string &parsing )
{
	string type = "";
	while ( isspace( parsing[0] ) )
		parsing.erase(0,1);
	while ( isalnum( parsing[0] ) )
	{
		type += parsing[0];
		parsing.erase(0,1);
	}
	return type;	// empty type for ERR
}
vector<string> parseVars ( string &parsing, ifstream &inf, int &line )
{
	vector<string> vars;
	string parsed_tmp;
	bool flag = true;

	while (flag)
	{
		if ( isalnum( parsing[0] ) )
			parsed_tmp += parsing[0];
		else if ( parsing[0] == "," || parsing[0] == ";")
		{
			case_reg.push_back(parsed_tmp);
			parsed_tmp = "";
			if ( parsing[0] == ";")
			{
				string nextLine = "";
				if ( !getline( inf, nextLine ) )
					flag = false;
				else parsing += nextLine;
				break;
			}
		}
		else if ( parsed_tmp != "" && isspace( parsing_str[0] ) )
			flag = false;

		if ( parsing.length() > 1 ) parsing.erase(0,1);
		else if ( getline( inf, parsing ) ) line++;
		else flag = false;
	}	// empty vector for ERR
	return (flag)? vars: vector<string>();
}