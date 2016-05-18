// Lempel-Ziv 77

#ifndef LZ77_H
#define LZ77_H

#include <fstream>

void lz77_c(std::ifstream& fin, std::ofstream& fout); // compress
void lz77_x(std::ifstream& fin, std::ofstream& fout); // extract

#endif
