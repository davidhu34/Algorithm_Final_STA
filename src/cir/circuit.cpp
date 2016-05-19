#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

bool moduleERR ( int line )
{ 
	cerr << "invalid module file format (line "
		<< line << ")."<<endl;
	return true;
}
bool regERR ( int line )
{
	cerr << "name not registerred in module (line "
		<< line << ")."<<endl;
	return true;

}

void Circuit::parseFile ( ifstream &inf )
{	// premise: first line starts with "module <case_name>"
	string case_name = "";
	vector<string> case_reg;

	int parsing_line;	// track error
	string parsing_str;
	string parsed_str = "";
	type = "";	// input / output / modle name 
	bool module_validation;	// error break


	if ( parsing_str.find("module ") == string::npos )
		module_validation = moduleERR();
	else
	{	// module initialized
		size_t caseNameStart;
		parsing_str.erase( 0, 7 );
 		// get case name
 		caseNameStart = parsing_str.find_first_not_of(" ");
 		if ( caseNameStart == string::npos ) moduleERR(parsing_line);
		else parsing_str.erase( 0, caseNameStart );
		caseNameEnd = parsing_str.find_first_of(" (")
		if ( caseNameEnd == string::npos ) moduleERR(parsing_line);
		else case_name = parsing_str.substr( 0, caseNameEnd );
		parsing_str.erase(0, parsing_str.find_first_of("(") + 1);
	}
	// register names
	type = "register";
	getnames( inf, parsing_str)

	while ( module_validation )
	{
		if  ( isspace( parsing_str[0] ) )
		{
			if ( parsed_str != "" && type == "" ) // check new type 
			{
				type = parsed_str;
				parsed_str = "";
				checker = ( inModel(type) )? ";": ",;";
			}
		}
		else if ( checker.find(parsing_str[0]) != string::npos )
		{	
			switch (type)
			{
				case "register":
					case_reg.push_back(parsed_str);
					break;
				case "input":
					newInput(parsed_str);
					break;
				case "output":
					newOutput(parsed_str);
					break;
				case "wire":
					newWire(parsed_str);
					break;
				default: // parse gate
					newGate(parsed_str);
			}
			parsed_str = "";
			if ( parsing_str[0] == ";") type = "";
		}
		else parsed_str += parsing_str[0];

		// check end of line & getline
		if ( parsing_str.length() > 1 ) parsing_str.erase(0,1);
		else if ( getline( inf, parsing_str ) ) line++;
		else if ( parsed_str == "endmodule" ) break;
		else moduleERR(line); 
	}
		
	}
}