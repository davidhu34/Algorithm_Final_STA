#ifndef HC_H
#define HC_H

// Huffman coding
//
// Compressed File Format
//   Info to rebuild coding table
//     [8-bit] number of symbol that has code of 1 bit length, followed by
//     [8-bit] number of symbol that has code of 2 bits length, followed by
//     ...
//     [8-bit] number of symbol that has code of k (k != 0) bits length
//     [8-bit] 0xff (a seperator)
//     [8-bit] character (called it c) --
//     [8-bit] character                | Character that has code length > 0
//     ...                              | sorted by code length, then by
//     [8-bit] character ---------------- character value.
//     [8-bit] c (a seperator)
//
//   Encoding
//     [?-bit] code
//     [?-bit] code
//     ...

#include <fstream>

// Compress using Huffman coding.
//
// Input
//   fin: A file.
//
// Output
//   fout: A compressed file.
//
void hc_c(std::ifstream& fin, std::ofstream& fout);

// Extract file compressed by Huffman coding.
//
// Input
//   fin: A file.
//
// Output
//   fout: An extracted file.
//
void hc_x(std::ifstream& fin, std::ofstream& fout);

#endif
