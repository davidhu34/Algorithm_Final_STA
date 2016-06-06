#include "sta/src/cir/parser.h"

namespace Sta {
namespace Cir {

using namespace std;

bool Parser::moduleERR ()
{ 
	cerr << "invalid module file format (line "
		<< _parsingLine << ")."<<endl;
	return false;
}

bool Parser::parseCase ()
{
	map< string, string> models = _ckt->getModels();

	if ( !getNextLine() )	return moduleERR();
//cout<<"got line 1"<<endl;
	if ( parseWord() != "module" )	return moduleERR();	// module initialized
//cout<<"got \"module\""<<endl;
	string caseName = parseWord();	// get case name
	if ( caseName == "" )   return moduleERR();
	_ckt->setCaseName(caseName);
//cout<<"got case name"<<endl;
	vector<string> varReg = parseVars();	// get parameters
	if ( varReg.size() == 0 )	moduleERR();

	while (1)
	{
		string token = parseWord();
		if ( token == "" ) return moduleERR();
//cout<<"got token: "<<token<<endl;
		if ( models.find(token) != models.end() ) {	
			if ( !parseGate(token) )	return moduleERR();
		} else if ( token == "endmodule") break;
		else {
			vector<string> varTmp = parseVars();
			if ( varTmp.empty() )	moduleERR();
			
			if ( token == "input" ) {
				for ( size_t i = 0; i < varTmp.size(); i++ )	_ckt->newInput( varTmp[i] );
			} else if ( token == "output" ) {
				for ( size_t i = 0; i < varTmp.size(); i++ )	_ckt->newOutput( varTmp[i] );
			} else if ( token == "wire" ) {
				for ( size_t i = 0; i < varTmp.size(); i++ )	_ckt->newWire( varTmp[i] );
			} else return moduleERR();		// invalid token 
		}
	}
	_ckt->connectGates();
	return true;
}

string Parser::parseWord ()
{
	string word = "";
	while ( _parsingStr == "" )	
		if ( !getNextLine() )	return "";
	while ( isspace( _parsingStr[0] ) )
		_parsingStr.erase(0,1);
	while ( isalnum( _parsingStr[0] ) )
	{
		word += _parsingStr[0];
		_parsingStr.erase(0,1);
	}
//cout<< "word: " << word << endl;
	return word;    // empty word for ERR
}

vector<string> Parser::parseVars ()
{
	vector<string> vars;
	string parsedTmp;
	bool flag = true;	// false flag: module ERR

//cout<<"vars: ";
	while (flag)
	{
		if ( isalnum( _parsingStr[0] ) )
			parsedTmp += _parsingStr[0];
		else if ( _parsingStr[0] == ',' || _parsingStr[0] == ';' )
		{
//cout<< parsedTmp<<_parsingStr[0]<<" ";
			vars.push_back(parsedTmp);
			parsedTmp = "";
			if ( _parsingStr[0] == ';' )
			{
				if ( !getNextLine() )	flag = false;
				break;
			}
		}
		if ( _parsingStr.length() > 1 ) _parsingStr.erase(0,1);
		else if ( !getNextLine() )	flag = false;
	}   // empty vector for ERR
//cout<<endl;
	return (flag)? vars: vector<string>();
}

bool Parser::parseGate ( string model )
{
	string gname, inputA, inputB, outputY;
	gname = parseWord();
	if ( gname == "" ) return false;
	inputA = trimWire("A");
	inputB = trimWire("B");
	outputY = trimWire("Y");
	//cout<<"Y:" <<outputY<<endl;
	return _ckt->newLogicGate( gname, model, inputA, inputB, outputY )
		&& getNextLine();
}

string Parser::trimWire ( string pin )
{
	size_t pStart, pEnd;
	pStart = _parsingStr.find( "." + pin +"(" );
	if ( pStart == string::npos )	return "";
	else pEnd = _parsingStr.find( ")", pStart + 3);
	if ( pEnd == string::npos )	return "";
	return _parsingStr.substr( pStart + 3, pEnd - pStart - 3 );
}

bool Parser::getNextLine ()
{
	if ( getline( _inf, _parsingStr ) )
	{
		//cout<<_parsingStr<<endl;
		_parsingLine++;
		size_t commentStart = _parsingStr.find("//");
//cout<<"comment found at: ";
//if( commentStart == string::npos ) cout<<"npos"<<endl;
//else cout <<commentStart <<endl;
		if ( commentStart != string::npos )
			_parsingStr.erase( _parsingStr.begin() + commentStart, _parsingStr.end() );
		else if ( commentStart == 0 ) return getNextLine();
		return true;
	} else return moduleERR();
}

} // namespace Cir
} // namespace Sta
