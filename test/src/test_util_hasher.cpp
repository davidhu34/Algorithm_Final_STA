#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

#include "sta/src/test/src/util/util.h"

#include "sta/src/util/hasher.h"

// Read in a file that contain many words. Take hash value of each of
// these words. Save these hash value into a table. Use a multimap to
// map word's hash value to itself. Print out each collision. If number
// of collision > certain critical value, this test is failed.
//
void test_hash_str(void) {
    std::cerr << __FUNCTION__ << "():\n";

    const char* filename = "test/cases/words.txt";

    std::ifstream fin(filename);
    ASSERT(fin.good(),
        << "Cannot open file \"" << filename << "\n");

    typedef std::multimap<uint32_t, std::string>::value_type     Pair;
    typedef std::multimap<uint32_t, std::string>::const_iterator Iterc;

    std::multimap<uint32_t, std::string> dict;
    std::vector<uint32_t>                vec;

    // Read each word and put them into `vec` and `dict`.
    std::string str;
    while (fin >> str) {
        uint32_t n = Sta::Util::hash_str(str);
        dict.insert((Pair(n, str)));
        vec.push_back(n);
    }

    // Calculate and print collitions.
    std::cerr << "Collisions:\n";

    int max_num_in_bucket = 0;

    for (size_t i = 0; i < vec.size(); ++i) {
        if (dict.count(vec[i]) > 1) {
            std::cout << vec[i] << ": ";

            std::pair<Iterc, Iterc> p = dict.equal_range(vec[i]);

            int num_in_bucket = 0;
            while (p.first != p.second) {
                std::cerr << p.first->second << " ";
                ++p.first;
                ++num_in_bucket;
            }
            std::cerr << "\n";

            if (num_in_bucket > max_num_in_bucket) {
                max_num_in_bucket = num_in_bucket;
            }
        }
    }

    ASSERT(max_num_in_bucket < 3,
        << "I think the implementation is wrong, too many collisions.\n");

    std::cerr << __FUNCTION__ << "() passed.\n";
}
