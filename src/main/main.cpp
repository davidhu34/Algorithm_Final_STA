#include <iostream>
#include <vector>

int main(int argc, const char* argv[]) {
    // TODO: Validate arguments.

    // Parse input file into circuit.
    
    Cir::Parser parser;
    Cir::Circuit circuit = parser.parse(argv[1]);

    // Find all sensitizable paths.
    
    typedef std::vector<const Cir::Circuit::Node*> Path;
    typedef std::vector<bool>                      InputVec;

    std::vector<Path>     paths;
    std::vector<int>      delays;
    std::vector<InputVec> input_vecs;

    Ana::Analyzer analyzer;
    analyzer.analyze(circuit, paths, delays, input_vecs);

    // Output those paths.

    std::string outfilename = argv[1];
    outfilename += "_true_path_set";

    std::ofstream fout(outfilename.c_str());
    if (!fout.good()) {
        std::cerr << "Error: Cannot open file '" << outfilename << "'.\n";
        return 1;
    }

    // TODO: Continue to print paths to file.

    return 0;
}
