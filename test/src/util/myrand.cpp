#include "myrand.h"

// Here's method of generating random number. 
//
// First, we initialize an array named mt with size 624. We assign a
// number (seed) to mt[0], then, for i = 1 to 623, we assign some
// transformation of mt[i - 1] to mt[i]. After that, we regenerate
// a set of numbers by mixing and throwing and shifting elements of
// mt arround, then assign this set of number into mt.
//
// Now, every time a person ask for a random number, we pick one from
// mt, do some transformation on it, and return it. We pick one by
// one, from mt[0] to mt[623]. We use an index to keep track of next
// one to be picked.
//
// After we picked from mt[623], the next one to be returned should be
// mt[0]. But this cycle is so short. So before returning mt[0], we need
// to regenerate a set of number again, then assign it to mt. Now we can
// return mt[0].

// ### Helpers

// Maximum possible random number.
static const unsigned randMax = 0xffffffffu;

// An array that put generated random numbers.
static unsigned mt[624];

// Keep track of picked number.
static unsigned index = 0;

// Check whether mt has been initialized.
static bool bootstrapped = false;

// Generate a set of numbers by mixing and throwing and shifting elements
// of mt, and then assign it back to mt.
//
static void generateNumbers(void) {
    for (unsigned i = 0; i < 624; ++i) {
        // Do some transform.
        unsigned y = (mt[i] & 0x80000000u) + 
                     (mt[(i + 1) % 624] & 0x7fffffffu);

        // Some more transform.
        mt[i] = mt[(i + 397) % 624] ^ (y >> 1);

        // Some more transform.
        if (y % 2) {
            mt[i] = mt[i] ^ 0x9908b0dfu;
        }
    }
}

// Pick a number from mt, do some transformation, and return it.
//
// #### Output
//
// - A random number.
//
static unsigned extractNumber(void) {
    // Regenerate number every time index = 0.
    if (index == 0) {
        generateNumbers();
    }
    
    // Pick a number.
    unsigned y = mt[index];

    // Do some transformation before returning it.
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & 0x9d2c5680u);
    y = y ^ ((y << 15) & 0xefc60000u);
    y = y ^ (y >> 18);

    ++index;
    index %= 624u;
    
    return y;
}

// Primary functions.

double TestUtil::myrand(void) {
    double n = (double)(mt19937()) / randMax;
    while (n == 1.0) n = (double)(mt19937()) / randMax;
    return n;
}


unsigned TestUtil::mt19937(void) {
    // bootstrap array mt[]
    if (!bootstrapped)
        mt19937init(13);

    return extractNumber();
}

unsigned TestUtil::mt19937init(unsigned seed) {
    bootstrapped = true;
    index = 0;
    mt[0] = seed;
    for (unsigned i = 1; i < 624; ++i) {
       mt[i] = 0x6c078965u * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i;
    }

    return seed;
}
