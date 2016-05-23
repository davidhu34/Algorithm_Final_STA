#ifndef STA_UTIL_HASH_MAP_H
#define STA_UTIL_HASH_MAP_H

#include <cassert>
#include <vector>

namespace Util {

// Hash map of key to value. Compare key using `operator==`.
//
// Hash is a hash function type for key, that has implement
// `size_t operator()(const Key&)`. It can also be a function pointer
// type, e.g. `size_t (*)(const Key&)`. It return a `size_t` type
// using full `size_t` full range.
//
// To loop through all element in hash map, you have to loop bucket
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
template <typename Key, typename Value, typename Hash>
struct hash_map {

    // Key-value pair.
    struct Pair {
        Key   key;
        Value value;

        // Constructor
        explicit Pair(const Key& _key): key(_key) {}

        Pair(const Key& _key, const Value& _value):
            key(_key),
            value(_value) {}
    };

    // Data Member
    std::vector< std::vector<Pair> > bucket;
    Hash                             hash;

    // Constructor. Bucket size is initialized to 1.
    //
    // #### input
    //
    // - `hash`: A hash function for key.
    //
    explicit hash_map(Hash _hash): bucket(1), hash(_hash) { }

    // Member functions

    // Insert a key-value pair.
    //
    // #### Input
    //
    // - `key`
    // - `value`
    //
    void insert(const Key& key, const Value& value) {
        size_t idx = hash(key) % bucket.size();
        bucket[idx].push_back((Pair(key, value)));
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
    bool erase(const Key& key) {
        size_t idx = hash(key) % bucket.size();
        for (size_t i = 0; i < bucket[idx].size(); ++i) {
            if (key == bucket[idx][i].key) {
                bucket[idx].erase(bucket[idx].begin() + i);
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

    // Check whether hash_map contains this key.
    //
    // #### Input
    //
    // - `key`:
    //
    // #### Output
    //
    // - true or fale.
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

    // Tell hash_map to rehash using with certain number of buckets.
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

#endif // STA_UTIL_HASH_MAP_H
