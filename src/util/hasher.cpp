//-----------------------------------------------------------------------------
// This is a slighly modified version of MurmurHash3.
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//-----------------------------------------------------------------------------

#include "sta/src/util/hasher.h"

uint32_t Sta::Util::hash_str(const std::string& str) {
    // #### Initialization

    // Get pointer to character array.
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&str[0]);

    // String length in number of bytes.
    const size_t len = str.size();

    // We will divide whole string into 32-bit blocks. Here's number of
    // blocks we need.
    const size_t nblocks = len/4;

    // `h1` is a helper. 13 is a random seed.
    uint32_t h1 = 13;

    // Magic numbers that can help us to randomize our number.
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // #### Body

    // Point to last block that may or may not contain tail part of string.
    // It depends on whether `len` is multiple of 4.
    const uint32_t* blocks = (const uint32_t*)(data + nblocks*4);

    //                                 blocks
    //                                 v
    // x x x x|x x x x|x x x x|x x x x|x x x      <-- byte
    // ^
    // blocks[i] where i is negative
    //
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15)); // Rotate left for 15 bits.
        k1 *= c2;
        
        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13)); // Rotate left for 13 bits.
        h1 = h1*5 + 0xe6546b64;
    }

    // #### Tail

    // Point to first byte of last block.
    const uint8_t* tail = (const uint8_t*)(data + nblocks*4);

    uint32_t k1 = 0;

    // Get remainder (tail part of string) of len/4.
    switch(len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1; 
                k1 = (k1 << 15) | (k1 >> (32 - 15));
                k1 *= c2;
                h1 ^= k1;
    };

    // #### Finalization

    h1 ^= len;
    
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}
