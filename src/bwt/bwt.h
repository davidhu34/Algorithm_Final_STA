// Burrows Wheel Transform

#ifndef BWT_H
#define BWT_H

#include <fstream>

void bwt_c(std::ifstream& fin, std::ofstream& fout); // compress
void bwt_x(std::ifstream& fin, std::ofstream& fout); // extract

#endif
