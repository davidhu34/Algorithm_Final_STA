#ifndef TEST_UTIL_UTIL_H
#define TEST_UTIL_UTIL_H

#include <cstdlib>
#include <iostream>
#include <string>

#ifndef NDEBUG

    // If condition failed, print out filename, line number and message,
    // then exit with error code. Message can be a stream, e.g.
    // `"Error: a != b. a is " << a << " while b is " << b << ".\n")`.
    // Anything can be read by std::cerr with `operator<<` can exist
    // in that stream.
    //
    // #### Input
    //
    // - `condition`: An expression to be evaluated.
    // - `message`  : Message to be printed if condition evaluated to
    //                false. It should start with `<<`. It can be empty.
    //
    // Add `break` or `continue` after your message if you don't want
    // to exit program after assertion failed, e.g.
    // `ASSERT(1 == 0, << "1 != 0\n"; continue)`. You will continue
    // to run current test function.
    //
    // Add return statement after your message if you just want to stop
    // this test function, i.e. `ASSERT(1, ; return)`. You will leave
    // this test function but continue to run other test function.
    //
    #define ASSERT(condition, message)                                \
        do {                                                          \
            if (! (condition)) {                                      \
                std::cerr << __FILE__ << ":" << __LINE__ << ":\n"     \
                             "Assertion '" #condition "' failed.\n"   \
                             message;                                 \
                exit(EXIT_FAILURE);                                   \
            }                                                         \
        } while (false)

#else
    // Define a dummy ASSERT()
    #define ASSERT(condition, message) do { } while (false)
    
#endif // NDEBUG

namespace TestUtil {

// Return a string that escape all whitespaces (exluding space) character.
//
// #### Input
//
// - str: A string.
//
// #### Output
//
// - A string with all white space escaped.
//
inline std::string escape(const std::string& str) {
    std::string temp;
    for (size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
            case '\v':
                temp.push_back('\\');
                temp.push_back('v');
                break;
            
            case '\t':
                temp.push_back('\\');
                temp.push_back('t');
                break;

            case '\n':
                temp.push_back('\\');
                temp.push_back('n');
                break;

            case '\f':
                temp.push_back('\\');
                temp.push_back('f');
                break;

            case '\r':
                temp.push_back('\\');
                temp.push_back('r');
                break;

            default:
                temp.push_back(str[i]);
                break;
        } // switch (str[i])
    } // for

    return temp;
}

} // namespace TestUtil

#endif // TEST_UTIL_UTIL_H
