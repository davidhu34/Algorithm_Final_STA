#ifndef TEST_UTIL_MYRAND_H
#define TEST_UTIL_MYRAND_H

namespace TestUtil {

// Return a double value in range [0, 1).
//
double myrand(void);

// Return an unsigned value in range [0, 2^32-1]
//
unsigned mt19937(void);

// Seed for mt19937.
//
// #### Input
//
// - seed
//
// #### Output
//
// - Input seed.
//
unsigned mt19937init(unsigned seed);

} // namespace TestUtil

#endif // TEST_UTIL_MYRAND_H
