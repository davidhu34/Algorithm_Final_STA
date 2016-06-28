#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "sta/test/src/util/util.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/prime.h"
#include "sta/src/util/hash_set.h"

void test_hash_set(void) {
    using Sta::Util::hash_str;
    using Sta::Util::prime_gt;

    const char* filename = "test/cases/words.txt";
    std::ifstream fin(filename);
    ASSERT(fin.good(), << "Cannot open file \"" << filename << "\"\n");

    typedef uint32_t (*Hasher)(const std::string&);
    typedef std::string                     Str;
    typedef Sta::Util::HashSet<Str, Hasher> HashSet;

    HashSet          dict(hash_str);
    std::vector<Str> words;
    Str              word;
    
    while (fin >> word) {
        words.push_back(word);
        dict.insert_blindly(word);
        ASSERT(dict.size == words.size(), );
    }

    dict.rehash(prime_gt(dict.size));

    for (size_t i = 0; i < words.size(); ++i) {
        ASSERT(dict.contains(words[i]), );
    }

    const HashSet& dictref = dict;

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
        dict.find_or_insert(words[i]);
        dict.remove(words[i]);
    }

    ASSERT(dict.size == 1, );
}

