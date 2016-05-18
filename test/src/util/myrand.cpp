#include "myrand.h"

static const unsigned randMax = 0xffffffffu;

double myrand(void) {
    double n = (double)(mt19937()) / randMax;
    while (n == 1.0) n = (double)(mt19937()) / randMax;
    return n;
}

static unsigned mt[624];
static unsigned index = 0;

static void generateNumbers(void) {
    for (unsigned i = 0; i < 624; ++i) {
        unsigned y = (mt[i] & 0x80000000u) + (mt[(i + 1) % 624] & 0x7fffffffu);
        mt[i] = mt[(i + 397) % 624] ^ (y >> 1);
        if (y % 2) mt[i] = mt[i] ^ 0x9908b0dfu;
    }
}

static unsigned extractNumber(void) {
    if (index == 0) generateNumbers();
    
    unsigned y = mt[index];
    y = y ^ (y >> 11);
    y = y ^ ((y << 7) & 0x9d2c5680u);
    y = y ^ ((y << 15) & 0xefc60000u);
    y = y ^ (y >> 18);

    ++index;
    index %= 624u;
    
    return y;
}

static bool bootstrapped = false;

unsigned mt19937(void) {
    // bootstrap array mt[]
    if (!bootstrapped)
        mt19937init(13);

    return extractNumber();
}

unsigned mt19937init(unsigned seed) {
    bootstrapped = true;
    index = 0;
    mt[0] = seed;
    for (unsigned i = 1; i < 624; ++i) {
       mt[i] = 0x6c078965u * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i;
    }

    return seed;
}
