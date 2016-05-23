#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

#include "cir/parser.inc"

void test_get_token(void) {
    std::cerr << "test_get_token():\n";

    std::istringstream sin(
        "haha.hoho, :;(what,who$1,sum_m&er\n)[/]=`*//hello\n*>//");

    std::string ans1 = 
        "haha . hoho , : ; ( what , who$1 , sum_m er ) [ / ] = ` * *> ";

    std::string ans2 =
        "haha . hoho , : ; ( what , who$1 , sum_m er \n ) [ / ] = ` * *> ";

    std::string out1, out2;

    std::string token;
    while ((token = get_token(sin)) != "") {
        out1 += token;
        out1 += " ";
    }

    assert(out1 == ans1 && "Error: out1 != ans1\n");

    sin.clear();
    sin.seekg(0);

    while ((token = get_token(sin, 1)) != "") {
        out2 += token;
        out2 += " ";
    }

    assert(out2 == ans2 && "Error: out2 != ans2\n");

    std::cerr << "test_get_token() passed.\n";
}
