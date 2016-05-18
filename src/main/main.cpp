#include <cstring>
#include <ctime>

#include <iostream>
#include <fstream>
#include <vector>

#include "hc.h"
#include "ac.h"
#include "lz77.h"
#include "lzw.h"
#include "bwt.h"

static void print_usage(void) {
    std::cerr << 
        "Usage:\n"
        "  compress -m <method> -<c|x> <input_file> <output_file>\n";
}

static std::ifstream::pos_type filesize(const char* filename) {
    std::ifstream fin(filename, std::ios_base::ate | std::ios_base::binary);
    return fin.tellg();
}

int main(int argc, const char* argv[]) {
    // Parse arguments.
    std::vector<const char*> files;
    
    enum Op {
        op_h, // huffman coding
        op_a, // arithmetic coding
        op_7, // lempel-ziv 77
        op_w, // lempel-ziv-welch
        op_b, // burrows wheel transform
        op_none
    } op = op_none;

    enum Mode {
        mo_c, // compress
        mo_x, // extract
        mo_none
    } mode = mo_none;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-m") == 0) {
            ++i;
            if (i >= argc) {
                std::cerr << "Error: Missing method after -m.\n";
                print_usage();
                return 1;
            }

            if      (strcmp(argv[i], "h") == 0) op = op_h;
            else if (strcmp(argv[i], "a") == 0) op = op_a;
            else if (strcmp(argv[i], "7") == 0) op = op_7;
            else if (strcmp(argv[i], "w") == 0) op = op_w;
            else if (strcmp(argv[i], "b") == 0) op = op_b;
            else {
                std::cerr << "Error: Method " << argv[i]
                          << " not recognized.\n";
                return 1;
            }
        }

        else if (strcmp(argv[i], "-c") == 0) mode = mo_c;
        else if (strcmp(argv[i], "-x") == 0) mode = mo_x;

        else {
            files.push_back(argv[i]);
        }
    }

    // Validate arguments.
    if (mode == mo_none) {
        std::cerr << "Error: Forget to select compress or extract.\n";
        print_usage();
        return 1;
    }
    if (op == op_none) {
        std::cerr << "Error: Forget to select compression method.\n";
        print_usage();
        return 1;
    }
    if (files.size() != 2) {
        std::cerr << "Error: Too few/many arguments (input/output file).\n";
        print_usage();
        return 1;
    }
    
    std::ifstream fin(files[0], std::ios_base::in | std::ios_base::binary);
    if (!fin) {
        std::cerr << "Error: Cannot open file \"" << files[0] << "\".\n";
        return 1;
    }

    std::ofstream fout(files[1], std::ios_base::out | std::ios_base::binary);
    if (!fout) {
        std::cerr << "Error: Cannot open file \"" << files[1] << "\".\n";
        return 1;
    }

    if (filesize(files[0]) == 0) {
        fin.close();
        fout.close();
        return 0;
    }

    // Call dedicated functions.
    clock_t beg = clock();

    switch (op) {
        case op_h: mode == mo_c ? hc_c(fin, fout)   : hc_x(fin, fout); break;
        case op_a: mode == mo_c ? ac_c(fin, fout)   : ac_x(fin, fout); break;
        case op_7: mode == mo_c ? lz77_c(fin, fout) : lz77_x(fin, fout); break;
        case op_w: mode == mo_c ? lzw_c(fin, fout)  : lzw_x(fin, fout); break;
        case op_b: mode == mo_c ? bwt_c(fin, fout)  : bwt_x(fin, fout); break;
    };

    clock_t end = clock();

    // Calculate and output some stats.
    double elapse = (double)(end - beg) / CLOCKS_PER_SEC;
    std::cout << elapse << "\n";

    fin.close();
    fout.close();

    if (mode == mo_c) {
        std::ifstream::streampos fin_n = filesize(files[0]);
        std::ifstream::streampos fout_n = filesize(files[1]);

        double ratio = (double)(fin_n) / fout_n;

        std::cout << fin_n << "\n"
                  << fout_n << "\n"
                  << ratio << "\n";
    }

    return 0;
}

