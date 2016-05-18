// Arithmetic coding

#ifndef AC_H
#define AC_H

#include <fstream>

void ac_c(std::ifstream& fin, std::ofstream& fout); // compress
void ac_x(std::ifstream& fin, std::ofstream& fout); // extract

#endif
