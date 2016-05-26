#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "test/src/util/util.h"
#include "util/hasher.h"
#include "util/prime.h"
#include "util/hash_map.h"

void test_hash_map(void) {
    std::cerr << __FUNCTION__ << "():\n";

    const char* filename = "test/cases/words.txt";
    std::ifstream fin(filename);
    ASSERT(fin.good(), << "Cannot open file \"" << filename << "\"\n");

    typedef uint32_t (*Hasher)(const std::string&);
    typedef std::string                     Str;
    typedef Util::HashMap<Str, int, Hasher> HashMap;

    HashMap          dict(Util::hash_str);
    std::vector<Str> words;
    Str              word;
    
    while (fin >> word) {
        words.push_back(word);
        dict.insert(word, 13);
        ASSERT(dict.size == words.size(), );
    }

    dict.rehash(Util::prime_gt(dict.size));

    for (size_t i = 0; i < words.size(); ++i) {
        dict[words[i]] = 30;
    }

    const HashMap& dictref = dict;

    for (size_t i = 0; i < words.size(); ++i) {
        ASSERT(dictref.contains(words[i]),
            << "words[" << i << "] = " << words[i] << "\n");
    }
    ASSERT(!dictref.contains("dianhenglau"),
        << "What? dictref contain my name???\n");
    
    dict.find_or_insert("dianhenglau");

    ASSERT(dictref.contains("dianhenglau"),
        << "dict should contain my name.\n");

    ASSERT(dict.size == words.size() + 1, );

    dict.find_or_insert("love");

    ASSERT(dict.size == words.size() + 1, );
    
    for (size_t i = 0; i < words.size(); ++i) {
        dict.find_or_insert(words[i]) = 23;
        dict.remove(words[i]);
    }

    ASSERT(dict.size == 1, );

    std::cerr << __FUNCTION__ << "() passed.\n";
}
