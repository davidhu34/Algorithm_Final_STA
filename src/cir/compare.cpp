#include "sta/src/cir/compare.h"

#include <iostream>
#include <fstream>

#include "sta/src/util/hash_map.h"
#include "sta/src/util/hash_set.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/prime.h"

typedef std::string                              Str;
typedef uint32_t (*HashStr)(const Str&);
typedef Sta::Util::HashSet<Str, HashStr>         StrSet;
typedef Sta::Util::HashMap<Str, StrSet, HashStr> TokenMap;

// Print set of strings.
//
static std::ostream& operator<<(std::ostream& out, 
                                const StrSet& str_set) {

    for (size_t i = 0; i < str_set.bucket.size(); ++i) {
        for (size_t j = 0; j < str_set.bucket[i].size(); ++j) {
            out << str_set.bucket[i][j] << " ";
        }
    }

    return out;
}

#define ASSERT(condition, errmsg)                \
    if (!(condition)) {                          \
        std::cerr << "Error: " errmsg << "\n";   \
        return 1;                                \
    }

#define EXPECT(condition, errmsg)                \
    if (!(condition)) {                          \
        std::cerr << "Error: " errmsg << "\n";   \
        return_code = 1;                         \
    }

int Sta::Cir::compare_dump(const std::string& file1,
                           const std::string& file2) {

    using Util::hash_str;
    using Util::prime_gt;

    std::cerr << "Start comparing " << file1 << " and " << file2 
              << "...\n";

    // Map gate name to a set of gates that it is connected to.
    TokenMap token_map(hash_str);
    Str      token;
    Str      key_token;
    int      return_code = 0;

    token_map.bucket.resize(prime_gt(256));
    std::cerr << "Reading " << file1 << "...\n";

    // Read file1 and put all tokens into token_map, so that we
    // can check whether there is a difference between two circuit.
    //
    std::ifstream fin(file1.c_str());
    ASSERT(fin.good(), << "Cannot open file \"" << file1 << "\".")

    while (fin >> token) {
        if (token[0] == '-') {
            key_token = token;

            ASSERT(token_map.insert(key_token, StrSet(hash_str)),
                << "Key token " << key_token << " duplicated.")

            if (static_cast<double>(token_map.size) /
                token_map.bucket.size()               > 1.0) {

                token_map.rehash(prime_gt(token_map.bucket.size() * 2));
            }
        }
        else {
            ASSERT(!key_token.empty(),
                << "Key token has not yet been initialized while "
                << "trying to insert value token '" << token << "'.")

            StrSet& str_set = token_map.find_or_insert(
                                  key_token, 
                                  StrSet(hash_str));

            ASSERT(str_set.insert(token),
                << "Value token '" << token << "' duplicated in "
                << "key token '" << key_token << "'.")
            
            if (static_cast<double>(str_set.size) / 
                str_set.bucket.size()               > 1.0) {
                
                str_set.rehash(prime_gt(str_set.bucket.size() * 2));
            }
        }
    }
    fin.close();
    fin.clear();

    std::cerr << "Reading " << file2 << "...\n";

    // Read file2. For each key token, check whether it is inside
    // token_map. For each value token of a key token, check whether
    // it is inside token_map[key_token]. If it is, remove it from
    // token_map[gate]. If it isn't, issue error message.
    // At the end, token_map[gate] should be empty for all gate.
    // Check token_map for non empty value token set. Issue an error
    // for each of them.
    //
    fin.open(file2.c_str());
    ASSERT(fin.good(), << "Cannot open file \"" << file2 << "\".");

    while (fin >> token) {
        if (token[0] == '-') {
            key_token = token;

            if (!token_map.contains(key_token)) {
                std::cerr << "Error: Couln't find key token '"
                          << key_token << "' in token_map.\n";

                return_code = 1;

                // Fast forward until next key token.
                char c;
                while (fin.get(c) && c != '-') { }

                if (!fin.good()) {
                    break;
                }
                fin.putback(c);
            }
        }
        else {
            EXPECT(token_map[key_token].remove(token),
                << "Value token '" << token << "' does not exist in " 
                << "token set of key token '" << key_token << "'.")
        }
    }

    // token_map[key_token] should be empty for all key_token.
    for (size_t i = 0; i < token_map.bucket.size(); ++i) {
        for (size_t j = 0; j < token_map.bucket[i].size(); ++j) {
            const TokenMap::Pair& p = token_map.bucket[i][j];

            EXPECT(p.value.size == 0,
                << "There are some value token left for key token '"
                << p.key << "': " << p.value)
        }
    }

    std::cerr << "End cir_compare.\n";
    if (return_code == 0) {
        std::cerr << "Both files are same.\n";
    }

    return return_code;
}
