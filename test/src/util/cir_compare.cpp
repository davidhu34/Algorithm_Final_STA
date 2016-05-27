#include "test/src/util/cir_compare.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#include "test/src/util/util.h"

// Print set of strings.
//
static std::ostream& operator<<(std::ostream& output, 
                                const std::set<std::string>& strset) {

    typedef std::set<std::string>::const_iterator Iter;

    for (Iter it = strset.begin(); it != strset.end(); ++it) {
        output << *it << " ";
    }

    return output;
}

void TestUtil::cir_compare(const std::string& file1,
                           const std::string& file2);

    std::cout << "Start comparing " << file1 << " and " << file2 
              << "...\n";

    typedef std::string                       Str;
    typedef std::set<Str>                     StrSet;
    typedef std::set<Str>::iterator           SetIter;
    typedef std::map<Str, StrSet>::value_type Pair;
    typedef std::map<Str, StrSet>::iterator   MapIter;

    // Map gate name to a set of gates that it is connected to.
    std::map<Str, StrSet> token_map;
    Str                   token;
    Str                   key_token;

    std::cout << "Reading " << file1 << "...\n";

    // Read file1 and put all tokens into token_map, so that we
    // can check whether there is a difference between two circuit.
    //
    std::ifstream fin(file1.c_str());
    ASSERT(fin.good(), << "Cannot open file \"" << file1 << "\".\n");

    while (fin >> token) {
        if (token[0] == '-') {
            key_token = token;

            std::pair<MapIter, bool> p = 
                token_map.insert((Pair(key_token, StrSet())));

            ASSERT(p.second,
                << "Key token " << p.first->first << " already "
                << "existed.\n");
        }
        else {
            std::pair<SetIter, bool> p = 
                token_map[key_token].insert(token);

            ASSERT(p.second,
                << "Value token " << *p.first << " already existed "
                << "in " << key_token << ".\n";);
        }
    }
    fin.close();
    fin.clear();

    std::cout << "Reading " << file2 << "...\n";

    // Read file2. For each key token, check whether it is inside
    // token_map. For each value token of a key token, check whether
    // it is inside token_map[key_token]. If it is, remove it from
    // token_map[gate]. If it isn't, issue error message.
    // At the end, token_map[gate] should be empty for all gate.
    // Check token_map for non empty value token set. Issue an error
    // for each of them.
    //
    fin.open(file2.c_str());
    ASSERT(fin.good(), << "Cannot open file \"" << file2 << "\".\n");

    while (fin >> token) {
        
        if (token[0] == '-') {
            key_token = token;

            MapIter it = token_map.find(key_token);

            ASSERT(it != token_map.end(),
                << "Couldn't find key token " << key_token
                << " in token_map.\n"; continue);
        }
        else {
            SetIter it = token_map[key_token].find(token);

            ASSERT(it != token_map[key_token].end(),
                << "Value token " << token << " does not exist in " 
                << "token set of " << key_token << ".\n"; continue);

            token_map[key_token].erase(it);
        }
    }

    // token_map[key_token] should be empty for all key_token.
    for (MapIter it = token_map.begin(); it != token_map.end(); ++it) {
        ASSERT(it->second.empty(),
            << "There are some value token left for " << it->first
            << ": " << it->second << "\n"; continue);
    }

    std::cout << "End cir_compare.\n";
}
