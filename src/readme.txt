Components

	Gate

		members:
			fanin (vector of Gate*)
			fanout (vector of Gate*)
			gate name
			gate model

		child class:
			NOT
			NOR
			NAND
			INPUT
			OUTPUT

		virtual functions:
			constructor
			connect I/O pin to Gate*(s)

	Wire
		from pin Y of Gate*,
		to some pin(s) of some Gate*(s)


Circuit

	members:
		Wires (Wire* mapped to wire names)
		Inputs (Gate* mapped to names
		Outputs (vector of Gate*)
		Logic Gates (vector of Gate*)

		case name
		model names mapped to gate type

	utilities:
		set gate I/O's with data from wires
		add gates(of different types), models, wires to circuit

Parser

	members:
		ifstream reference
		Circuit* receiving data
		temporary storage for parsing file

	utilities:
		parse given case file data into Circuit

Writer

	members:
		ofstream reference
		Circuit* data source
		temporary storage for writing file

	utilities:
		write path data using given output format in the problem

Analyzer
