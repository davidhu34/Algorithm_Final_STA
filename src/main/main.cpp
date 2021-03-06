#include <cstring>   // strcmp()
#include <cstdlib>   // atoi(), exit()
#include <cctype>    // isdigit()
#include <iostream>
#include <fstream>
#include <vector>

#include "sta/src/cir/circuit.h"
#include "sta/src/cir/parser.h"
#include "sta/src/cir/writer.h"
#include "sta/src/cir/compare.h"
#include "sta/src/ana/analyzer.h"

static void print_usage(void) {
    std::cerr << "Usage:                                 \n\n"

                 "sta find -t <time_constraint>          \n"
                 "         -s <slack_constraint>         \n"
                 "         -o <output_file>              \n"
                 "         [-d <dump_file>]              \n"
                 "         <circuit_file>                \n\n"
                 
                 "sta verify -t <time_constraint>        \n"
                 "           -s <slack_constraint>       \n"
                 "           -v <true_path_set_file>     \n"
                 "           [-d <dump_file>]            \n"
                 "           <circuit_file>              \n\n"
                 
                 "sta dump [-d <dump_file>]              \n"
                 "         <circuit_file>                \n\n"
                 
                 "sta compare <dump_file_1> <dump_file_2>\n";
}

// Check wheter a string is an integer (can be negative).
// Do not accept a plus sign.
// 
// #### Input
//
// - `str`
//
// #### Output
//
// - True if it is an integer. False otherwise.
// - Return false if `str` is empty.
//
static bool is_number(const char* str) {
    if (str[0] == 0) { // Empty string.
        return false;
    }

    if (str[0] == '-') { // Negative number.
        ++str;
    }

    while (*str != 0) {
        if (!isdigit(*str)) {
            return false;
        }

        ++str;
    }

    return true;
}

// If condition evaluate to false, print error message and exit.
//
#define ASSERT(condition, errmsg)                \
    if (!(condition)) {                          \
        std::cerr << "Error: " errmsg << "\n";   \
        print_usage();                           \
        exit(1);                                 \
    }

// If condition evaluate to false, print error message and exit.
//
#define EXPECT(condition, errmsg)                \
    if (!(condition)) {                          \
        std::cerr << "Error: " errmsg << "\n";   \
        exit(1);                                 \
    }

static void execute_find(int argc, const char* argv[]) {
    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::Parser;
    using Sta::Cir::Writer;
    using Sta::Ana::find_true_paths;

    const char* infile               = 0;
    const char* outfile              = 0;
    const char* time_constraint_str  = 0;
    const char* slack_constraint_str = 0;
    const char* dump_file            = 0;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-o'.")
            outfile = argv[i];
        }
        else if (strcmp(argv[i], "-t") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-t'.")
            time_constraint_str = argv[i];
        }
        else if (strcmp(argv[i], "-s") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-s'.")
            slack_constraint_str = argv[i];
        }
        else if (strcmp(argv[i], "-d") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-d'.")
            dump_file = argv[i];
        }
        else { // Read input file names.
            ASSERT(!infile,
                << "Input file has already been provided:\n"
                << "Old: " << infile << "\n"
                << "New: " << argv[i])

            infile = argv[i];
        }
    }

    // Check arguments.

    ASSERT(infile,               << "Missing <input_file>.")
    ASSERT(time_constraint_str,  << "Missing '-t <time_constraint>'.")
    ASSERT(slack_constraint_str, << "Missing '-s <slack_constraint>'.")
    ASSERT(outfile,              << "Missing '-o <output_file>'.")

    ASSERT(is_number(time_constraint_str), 
        << "'" << time_constraint_str << "' is not a number.")

    ASSERT(is_number(slack_constraint_str), 
        << "'" << slack_constraint_str << "' is not a number.")

    int time_constraint  = atoi(time_constraint_str);
    int slack_constraint = atoi(slack_constraint_str);

    // Pass input files into circuit.
    
    Circuit circuit;

    std::ifstream inf(infile);
    EXPECT(inf.good(),
        << "Cannot open '" << infile << "'.\n")

    Parser parser(inf, &circuit);

    EXPECT(parser.parseCase(), 
        << "Parsing failed.")

    // Dump file if -d <dump_file> is specified.

    if (dump_file) {
        std::ofstream outf(dump_file);
        EXPECT(outf.good(),
            << "Cannot open '" << dump_file << "'.")

        std::streambuf* coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(outf.rdbuf());

        circuit.printState();

        std::cout.rdbuf(coutbuf);
    }

    // Find all true paths.
    
    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;

    int return_code = find_true_paths(
                          circuit, 
                          time_constraint,
                          slack_constraint,
                          paths, 
                          values, 
                          input_vecs);

    EXPECT(return_code == 0, << "Find true paths failed.")

    // Output those paths.

    std::ofstream outf(outfile);
    EXPECT(outf.good(),
        << "Cannot open '" << outfile << "'.")

    Writer writer(outf, &circuit);
    writer.setConstraint(time_constraint, slack_constraint);
    
    return_code = writer.writeTruePath(paths, 
                                       values, 
                                       input_vecs);

    EXPECT(return_code, << "Write to file failed.")

    // Clean up.

    circuit.clear();
}

static void execute_verify(int argc, const char* argv[]) {
    using Sta::Cir::Circuit;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Sta::Cir::Parser;
    using Sta::Cir::parse_true_path_set;
    using Sta::Ana::verify_true_path_set;

    const char* infile               = 0;
    const char* time_constraint_str  = 0;
    const char* slack_constraint_str = 0;
    const char* dump_file            = 0;
    const char* true_path_set_file   = 0;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-v'.")
            true_path_set_file = argv[i];
        }
        else if (strcmp(argv[i], "-t") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-t'.")
            time_constraint_str = argv[i];
        }
        else if (strcmp(argv[i], "-s") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-s'.")
            slack_constraint_str = argv[i];
        }
        else if (strcmp(argv[i], "-d") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-d'.")
            dump_file = argv[i];
        }
        else { // Read input file names.
            ASSERT(!infile,
                << "Input file has already been provided:\n"
                << "Old: " << infile << "\n"
                << "New: " << argv[i])

            infile = argv[i];
        }
    }

    // Check arguments.

    ASSERT(infile,               << "Missing <input_file>.")
    ASSERT(true_path_set_file,   << "Missing '-v <true_path_set_file>'.")
    ASSERT(time_constraint_str,  << "Missing '-t <time_constraint>'.")
    ASSERT(slack_constraint_str, << "Missing '-s <slack_constraint>'.")

    ASSERT(is_number(time_constraint_str), 
        << "'" << time_constraint_str << "' is not a number.")

    ASSERT(is_number(slack_constraint_str), 
        << "'" << slack_constraint_str << "' is not a number.")

    int time_constraint  = atoi(time_constraint_str);
    int slack_constraint = atoi(slack_constraint_str);

    // Parse verilog files into circuit.
    
    Circuit circuit;
    
    std::ifstream inf(infile);
    EXPECT(inf.good(),
        << "Cannot open '" << infile << "'.\n")

    Parser parser(inf, &circuit);

    EXPECT(parser.parseCase(), 
        << "Parsing failed.")

    // Dump file if -d <dump_file> is specified.

    if (dump_file) {
        std::ofstream outf(dump_file);
        EXPECT(outf.good(),
            << "Cannot open '" << dump_file << "'.")

        std::streambuf* coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(outf.rdbuf());

        circuit.printState();

        std::cout.rdbuf(coutbuf);
    }

    // Parse true_path_set_file into paths, values and input_vecs.

    std::vector<Path>                paths;
    std::vector< std::vector<bool> > values;
    std::vector<InputVec>            input_vecs;

    int return_code = parse_true_path_set(true_path_set_file,
                                          circuit,
                                          time_constraint,
                                          slack_constraint,
                                          paths,
                                          values,
                                          input_vecs);

    EXPECT(return_code == 0, 
        << "Parse '" << true_path_set_file << "' failed.")

    // Verify true path set.
    
    return_code = verify_true_path_set(circuit,
                                       time_constraint,
                                       slack_constraint,
                                       paths,
                                       values,
                                       input_vecs);
                                       
    EXPECT(return_code == 0, << "Verification process failed.")

    // Clean up.

    circuit.clear();
}

static void execute_dump(int argc, const char* argv[]) {
    using Sta::Cir::Circuit;
    using Sta::Cir::Parser;

    const char* infile    = 0;
    const char* dump_file = 0;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0) {
            ++i;
            ASSERT(i < argc, << "Missing argument after '-d'.")
            dump_file = argv[i];
        }
        else { // Read input file names.
            ASSERT(!infile,
                << "Input file has already been provided:\n"
                << "Old: " << infile << "\n"
                << "New: " << argv[i])

            infile = argv[i];
        }
    }

    // Check arguments.

    ASSERT(infile, << "Missing <input_file>.")

    // Parse input files into circuit.
    
    Circuit circuit;
    
    std::ifstream inf(infile);
    EXPECT(inf.good(),
        << "Cannot open '" << infile << "'.\n")

    Parser parser(inf, &circuit);

    EXPECT(parser.parseCase(), 
        << "Parsing failed.")

    // Dump file.

    if (dump_file) {
        std::ofstream outf(dump_file);
        EXPECT(outf.good(),
            << "Cannot open '" << dump_file << "'.")

        std::streambuf* coutbuf = std::cout.rdbuf();
        std::cout.rdbuf(outf.rdbuf());

        circuit.printState();

        std::cout.rdbuf(coutbuf);
    }
    else {
        circuit.printState();
    }

    // Clean up.

    circuit.clear();
}

static void execute_compare(int argc, const char* argv[]) {
    using Sta::Cir::compare_dump;

    const char* dump_file_1 = 0;
    const char* dump_file_2 = 0;

    ASSERT(argc >= 3, << "Missing <dump_file_1> and <dump_file_2>.")
    ASSERT(argc >= 4, << "Missing <dump_file_2>.")
    ASSERT(argc <= 4, << "Excess arguments after '" << argv[3] << "'.")

    dump_file_1 = argv[2];
    dump_file_2 = argv[3];

    compare_dump(dump_file_1, dump_file_2);
}

int main(int argc, const char* argv[]) {
    // Parse arguments.

    ASSERT(argc >= 2, << "Missing command.")

    if (strcmp(argv[1], "find") == 0) {
        execute_find(argc, argv);
    }
    else if (strcmp(argv[1], "verify") == 0) {
        execute_verify(argc, argv);
    }
    else if (strcmp(argv[1], "dump") == 0) {
        execute_dump(argc, argv);
    }
    else if (strcmp(argv[1], "compare") == 0) {
        execute_compare(argc, argv);
    }
    else {
        ASSERT(0, << "Command '" << argv[1] << "' not recognized.")
    }

    return 0;
}
