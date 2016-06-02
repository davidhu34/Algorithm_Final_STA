#include <cstring>   // strcmp()
#include <cstdlib>   // atoi()
#include <iostream>
#include <vector>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/ana/analyzer.h"

static void print_usage(void) {
    std::cerr << "Usage:\n"
                 "    sta -t <time_constraint>\n"
                 "        -s <slack_constraint>\n"
                 "        [-o <output_file>]\n"
                 "        <input_files>\n";
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
    const char*              time_constraint_str = 0;
    const char*              slack_constraint_str = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
            if (i >= argc) {
                std::cerr << "Error: Missing argument after -o.\n";
                print_usage();
                return 1;
            }
            outfile = argv[i];
        }
        else if (strcmp(argv[i], "-t") == 0) {
            ++i;
            if (i >= argc) {
                std::cerr << "Error: Missing argument after -t.\n";
                print_usage();
                return 1;
            }
            time_constraint_str = argv[i];
        }
        else if (strcmp(argv[i], "-s") == 0) {
            ++i;
            if (i >= argc) {
                std::cerr << "Error: Missing argument after -s.\n";
                print_usage();
                return 1;
            }
            slack_constraint_str = argv[i];
        }
        else { // Read input file names.
            infiles.push_back(argv[i]);
        }
    }

    // Check arguments.

    if (infiles.empty()) {
        std::cerr << "Error: Missing <input_file>.\n";
        print_usage();
        return 1;
    }
    if (!time_constraint_str) {
        std::cerr << "Error: Missing -t <time_constraint>.\n";
        print_usage();
        return 1;
    }
    int time_constraint = atoi(time_constraint_str);

    if (!slack_constraint_str) {
        std::cerr << "Error: Missing -s <slack_constraint>.\n";
        print_usage();
        return 1;
    }
    int slack_constraint = atoi(slack_constraint_str);

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
                                      time_constraint,  // Macro
                                      slack_constraint, // Macro
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
                        time_constraint, 
                        slack_constraint, 
                        paths, 
                        values, 
                        input_vecs, 
                        outfile);
    }
    else {
        errcode = write(circuit, 
                        time_constraint, 
                        slack_constraint, 
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
