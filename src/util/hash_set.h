#ifndef STA_UTIL_HASH_SET_H
#define STA_UTIL_HASH_SET_H

#include <cassert>
#include <vector>

namespace Sta {
namespace Util {

// HashSet of key. Compare key using `operator==`.
//
// Hash is a hash function type for key, that has implement
// `size_t operator()(const Key&)`. It can also be a function pointer
// type, e.g. `size_t (*)(const Key&)`. It return a `size_t` type
// using full `size_t` full range.
//
// To loop through all element in HashSet, you have to loop bucket
// yourself, e.g.
//
// ```
// for (size_t i = 0; i < foo.bucket.size(); ++i) {
//     for (size_t j = 0; j < foo.bucket[i].size(); ++j) {
//         print(foo.bucket[i][j])
//     }
// }
// ```
//
// You can use `Util::prime_gt(n)` to get a prime number greater
// than `n`, and use that prime number as bucket size to rehash.
// Prime number can help hash key to distribute more evenly.
//
// HashSet won't grow automatically. You need to grow it yourself.
//
template <typename Key, typename Hash>
struct HashSet {

    // Data Member
    std::vector< std::vector<Key> > bucket;
    Hash                            hash;
    size_t                          size;   // Read-only.

    // Constructor. Bucket size is initialized to 1.
    //
    // #### input
    //
    // - `hash`: A hash function for key.
    //
    explicit HashSet(const Hash& _hash): 
        bucket(1    ), 
        hash  (_hash), 
        size  (0    ) { }

    // Member functions

    // Insert a key. It will check before insert. If the key is already
    // exist, it will return false; Else it will insert that key and
    // return true.
    //
    // #### Input
    //
    // - `key`
    // - `value`
    //
    // #### Ouput
    //
    // Return true if the key-value pair is not in the set originally.
    // Return false otherwise.
    //
    bool insert(const Key& key) {
        size_t       idx   = hash(key) % bucket.size();
        const size_t bsize = bucket[idx].size();

        for (size_t i = 0; i < bsize; ++i) {
            if (key == bucket[idx][i]) {
                return false;
            }
        }
        bucket[idx].push_back(key);
        ++size;
        return true;
    }

    // Insert a key. It won't check whether HashSet contain `key`. 
    // It just hash that key and insert that key into bucket directly.
    // If you want to check whether HashSet contain `key` before insert,
    // use `find_or_insert()`.
    //
    // #### Input
    //
    // - `key`
    //
    void insert_blindly(const Key& key) {
        size_t idx = hash(key) % bucket.size();
        bucket[idx].push_back(key);
        ++size;
    }

    // Remove a key.
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
            if (key == bucket[idx][i]) {
                bucket[idx].erase(bucket[idx].begin() + i);
                --size;
                return true;
            }
        }
        return false;
    }

    // Try to find value with key and return it. If key cannot be found,
    // insert that key with value into HashSet.
    //
    // #### Input
    //
    // - `key`:
    // - `value`:
    //
    // #### Output
    //
    // Return reference to key.
    //
    Key& find_or_insert(const Key& key) {
        size_t       idx   = hash(key) % bucket.size();
        const size_t bsize = bucket[idx].size();

        for (size_t i = 0; i < bsize; ++i) {
            if (key == bucket[idx][i]) {
                return bucket[idx][i];
            }
        }
        bucket[idx].push_back(key);
        ++size;
        return bucket[idx][bsize];
    }

    // Check whether HashSet contains this key.
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
            if (key == bucket[idx][i]) {
                return true;
            }
        }
        return false;
    }

    // Tell HashSet to rehash using with certain number of buckets.
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

        std::vector< std::vector<Key> > temp_bucket(bucket);

        bucket.clear();
        bucket.resize(number_of_bucket);

        // Move key from temp_bucket to bucket.
        for (size_t i = 0; i < temp_bucket.size(); ++i) {
            for (size_t j = 0; j < temp_bucket[i].size(); ++j) {
                size_t idx = hash(temp_bucket[i][j]) % bucket.size();
                bucket[idx].push_back(temp_bucket[i][j]);
            }
        }
    }
};

} // namespace Util
} // namespace Sta

#endif // STA_UTIL_HASH_SET_H

