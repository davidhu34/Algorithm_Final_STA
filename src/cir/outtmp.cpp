void outputTruePathLists ( vector<Path*> paths )
{
	cout
	<< "Header  {  A True Path Set  }  " << endl << endl
	<< "  Benchmark  {  " << case_name << "  }" << endl << endl

	for ( size_t i = 0; i < paths.size(); i++ )
	{
		Path* path = paths[i];
		cout
	<< "  Path  {  " << i << "  }" << endl << endl
	<< "  A True Path List" << endl
	<< "  {" << endl
	<< "  ---------------------------------------------------------------------------" << endl
	<< "  Pin    type                                Incr        Path delay"<< endl;
	<< "  ---------------------------------------------------------------------------" << endl;

		vector<Gate*> wires = path->_wires;
		for ( size_t wi = 0; wi < wires.size(); wi++ )
		{
			cout
	<< "  " << setw(44) << wires[wi]->_name << "/" " (" << wires[wi]->_model << ")"
		<< wires[wi]->_inputValue << setw(11) << 
		

		} //end of Gates

		cout
	<< "  ---------------------------------------------------------------------------" << endl
	<< "    Data Required Time" << setw(12) << path->_dataReqTime << endl
	<< "    Data Arrival Time" << setw(13) << path->_dataArrivalTime << endl
	<< "    ---------------------------------------------------------------------------" << endl
	<< "    Slack" << setw(25) << path->_slack << endl
	<< "  }" << endl << endl
	<< "  Input Vector" << endl
	<< "  {" << endl;
	
		map<string, char> inputValue = path->_inputValue;
		for ( map< string, char>::iterator mit = inputValue.begin();
			mit != inputValue.end(); mit++ ) {
			cout
	<< "  " << setw(4) << mit->first << "  =  " << mit->second << endl;
		}	// end of Inputs

		cout
	<< "  }" << endl << endl;
	}	// end of Path

	cout
	<< "}" << endl;

}

--------------------------------------------