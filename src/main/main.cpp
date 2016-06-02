#include <cstring>   // strcmp()
#include <iostream>
#include <vector>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/ana/analyzer.h"

static void print_usage(void) {
    std::cerr << "Usage:\n  sta [-o <output_file>] <input_file> ...\n";
}

int main(int argc, const char* argv[]) {
    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::parse;
    using Sta::Cir::write;
    using Sta::Ana::find_sensitizable_paths;

    // Parse arguments.

    std::vector<const char*> infiles;
    const char*              outfile = 0;

    for (int i = 1; i < argc; ++i) {
        // Read -o and its argument.
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
            if (i >= argc) {
                std::cerr << "Error: Missing argument after -o.\n";
                print_usage();
                return 1;
            }

            outfile = argv[i];
        }
        // Read input file names.
        else {
            infiles.push_back(argv[i]);
        }
    }

    // Check arguments.

    if (infiles.empty()) {
        std::cerr << "Error: Missing <input_file>.\n";
        print_usage();
        return 1;
    }

    // Pass input files into circuit.
    
    Circuit circuit;
    
    int errcode = parse(infiles, circuit);
    if (errcode != 0) {
        std::cerr << "Error: Parsing failed.\n";
        return 1;
    }

    // Find all sensitizable paths.
    
    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;

    errcode = find_sensitizable_paths(circuit, 
                                      TIME_CONSTRAINT,  // Macro
                                      SLACK_CONSTRAINT, // Macro
                                      paths, 
                                      values, 
                                      input_vecs);
    if (errcode != 0) {
        std::cerr << "Error: Finding failed.\n";
        return 1;
    }

    // Output those paths.

    if (outfile) { // User specify output file.
        errcode = write(circuit, 
                        TIME_CONSTRAINT, 
                        SLACK_CONSTRAINT, 
                        paths, 
                        values, 
                        input_vecs, 
                        outfile);
    }
    else {
        errcode = write(circuit, 
                        TIME_CONSTRAINT, 
                        SLACK_CONSTRAINT, 
                        paths, 
                        values, 
                        input_vecs);
    }

    if (errcode != 0) {
        std::cerr << "Error: Write to file failed.\n";
        return 1;
    }

    return 0;
}
