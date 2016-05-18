// Lempel-Ziv-Welch

#ifndef LZW_H
#define LZW_H

#include <fstream>

void lzw_c(std::ifstream& fin, std::ofstream& fout); // compress
void lzw_x(std::ifstream& fin, std::ofstream& fout); // extract

#endif
