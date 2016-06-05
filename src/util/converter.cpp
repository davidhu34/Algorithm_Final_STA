#include "sta/src/util/converter.h"

std::string Sta::Util::to_str(size_t n) {
    std::string temp;
    do {
        temp += static_cast<char>(n % 10 + '0');
    } while (n /= 10);

    return std::string(temp.rbegin(), temp.rend());
}

std::string Sta::Util::to_str(int n) {
    int n_copy = n;
    n = n < 0 ? -n : n;

    std::string temp;
    do {
        temp += static_cast<char>(n % 10 + '0');
    } while (n /= 10);

    if (n_copy < 0) {
        temp += '-';
    }

    return std::string(temp.rbegin(), temp.rend());
}
