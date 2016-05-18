#ifndef MYRAND_H
#define MYRAND_H

/// Return double value in [0, 1).
double myrand(void);

/// Return unsigned value in [0, 2^32 - 1]
unsigned mt19937(void);

/// Seed for mt19937
/// Return seed
unsigned mt19937init(unsigned seed);

#endif