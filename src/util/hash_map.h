#ifndef STA_UTIL_HASH_MAP_H
#define STA_UTIL_HASH_MAP_H

#include <cassert>
#include <vector>

namespace Sta {
namespace Util {

// HashMap of key to value. Compare key using `operator==`.
//
// Hash is a hash function type for key, that has implement
// `size_t operator()(const Key&)`. It can also be a function pointer
// type, e.g. `size_t (*)(const Key&)`. It return a `size_t` type
// using full `size_t` full range.
//
// To loop through all element in HashMap, you have to loop bucket
// yourself, e.g.
//
// ```
// for (size_t i = 0; i < foo.bucket.size(); ++i) {
//     for (size_t j = 0; j < foo.bucket[i].size(); ++j) {
//         print(foo.bucket[i][j].key, foo.bucket[i][j].value)
//     }
// }
// ```
//
// You can use `Util::prime_gt(n)` to get a prime number greater
// than `n`, and use that prime number as bucket size to rehash.
// Prime number can help hash key to distribute more evenly.
//
// HashMap won't grow automatically. You need to grow it yourself.
//
template <typename Key, typename Value, typename Hash>
struct HashMap {

    // Key-value pair.
    struct Pair {
        Key   key;
        Value value;

        // Constructor
        Pair(const Key& _key, const Value& _value):
            key  (_key),
            value(_value) {}
    };

    // Data Member
    std::vector< std::vector<Pair> > bucket;
    Hash                             hash;
    size_t                           size;

    // Constructor. Bucket size is initialized to 1.
    //
    // #### input
    //
    // - `hash`: A hash function for key.
    //
    explicit HashMap(const Hash& _hash): 
        bucket(1    ), 
        hash  (_hash), 
        size  (0    ) { }

    // Member functions

    // Insert a key-value pair. It will check before insert. If the
    // key-value pair is already exist, it will return false; Else
    // it will insert that key-value pair and return true.
    //
    // #### Input
    //
    // - `key`
    // - `value`
    //
    // #### Ouput
    //
    // Return true if the key-value pair is not in the Map originally.
    // Return false otherwise.
    //
    bool insert(const Key& key, const Value& value) {
        size_t       idx   = hash(key) % bucket.size();
        const size_t bsize = bucket[idx].size();

        for (size_t i = 0; i < bsize; ++i) {
            if (key == bucket[idx][i].key) {
                return false;
            }
        }
        bucket[idx].push_back((Pair(key, value)));
        ++size;
        return true;
    }

    // Insert a key-value pair. It won't check whether HashMap contain
    // `key`. It just hash that key and insert that key-value pair into
    // bucket directly. If you want to check whether HashMap contain
    // `key` before insert, use `insert()`.
    //
    // #### Input
    //
    // - `key`
    // - `value`
    //
    void insert_blindly(const Key& key, const Value& value) {
        size_t idx = hash(key) % bucket.size();
        bucket[idx].push_back((Pair(key, value)));
        ++size;
    }

    // Remove a key-value pair.
    //
    // #### Input
    //
    // - `key`
    //
    // #### Output
    //
    // - `true` if key is found and removed, `false` otherwise
    //
    bool remove(const Key& key) {
        size_t idx = hash(key) % bucket.size();
        for (size_t i = 0; i < bucket[idx].size(); ++i) {
            if (key == bucket[idx][i].key) {
                bucket[idx].erase(bucket[idx].begin() + i);
                --size;
                return true;
            }
        }
        return false;
    }

    // Access value with key. It abort program when it can't find the
    // key, so make sure the key exists before you access it.
    //
    // #### Input
    //
    // - `key`
    //
    // #### Output
    //
    // - Reference to value.
    //
    Value& operator[](const Key& key) {
        size_t idx = hash(key) % bucket.size();
        for (size_t i = 0; i < bucket[idx].size(); ++i) {
            if (key == bucket[idx][i].key) {
                return bucket[idx][i].value;
            }
        }
        assert(false && "Error: Can't find key.");
    }
    const Value& operator[](const Key& key) const {
        size_t idx = hash(key) % bucket.size();
        for (size_t i = 0; i < bucket[idx].size(); ++i) {
            if (key == bucket[idx][i].key) {
                return bucket[idx][i].value;
            }
        }
        assert(false && "Error: Can't find key.");
    }

    // Try to find value with key and return it. If key cannot be found,
    // insert that key with value into HashMap.
    //
    // #### Input
    //
    // - `key`:
    // - `value`:
    //
    // #### Output
    //
    // - Reference to value.
    //
    Value& find_or_insert(const Key& key, const Value& value = Value()) {
        size_t       idx   = hash(key) % bucket.size();
        const size_t bsize = bucket[idx].size();

        for (size_t i = 0; i < bsize; ++i) {
            if (key == bucket[idx][i].key) {
                return bucket[idx][i].value;
            }
        }
        bucket[idx].push_back((Pair(key, value)));
        ++size;
        return bucket[idx][bsize].value;
    }

    // Check whether HashMap contains this key.
    //
    // #### Input
    //
    // - `key`:
    //
    // #### Output
    //
    // - true or false.
    //
    bool contains(const Key& key) const {
        size_t idx = hash(key) % bucket.size();
        for (size_t i = 0; i < bucket[idx].size(); ++i) {
            if (key == bucket[idx][i].key) {
                return true;
            }
        }
        return false;
    }

    // Tell HashMap to rehash using with certain number of buckets.
    // Number of bucket better be a prime number, so that two different
    // number, after dividing by it, will get two different remainder
    // with higher probability.
    //
    // #### Input
    //
    // `number_of_bucket`:
    //
    void rehash(size_t number_of_bucket) {
        assert(number_of_bucket && "Error: Number of bucket must > 0.");

        std::vector< std::vector<Pair> > temp_bucket(bucket);

        bucket.clear();
        bucket.resize(number_of_bucket);

        // Move key-value pair from temp_bucket to bucket.
        for (size_t i = 0; i < temp_bucket.size(); ++i) {
            for (size_t j = 0; j < temp_bucket[i].size(); ++j) {
                size_t idx = hash(temp_bucket[i][j].key) % bucket.size();
                bucket[idx].push_back((Pair(
                    temp_bucket[i][j].key,
                    temp_bucket[i][j].value)));
            }
        }
    }
};

} // namespace Util
} // namespace Sta

#endif // STA_UTIL_HASH_MAP_H

