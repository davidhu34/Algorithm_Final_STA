#ifndef STA_UTIL_BACKUP_H
#define STA_UTIL_BACKUP_H

#include "sta/src/util/hash_map.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/prime.h"

namespace Sta {
namespace Util {

template <typename T>
struct Backup {
    HashMap<T*, T, PtrHashFunc> items;

    Backup(): items(hash_ptr) { }

    // It will recover all items to original state when they were
    // synchronized.
    //
    ~Backup() {
        for (size_t i = 0; i < items.bucket.size(); ++i) {
            for (size_t j = 0; j < items.bucket[i].size(); ++j) {
                typename HashMap<T*, T, PtrHashFunc>::Pair& pair = 
                    items.bucket[i][j];
                *pair.key = pair.value;
            }
        }
    }

    void sync(T* item) {
        items.find_or_insert(item, *item) = *item;

        if (items.bucket.size() <= items.size) {
            items.rehash(prime_gt(items.size * 2));
        }
    }

    void sync_if_no_copy(T* item) {
        if (!items.contains(item)) {
            sync(item);
        }
    }
};

} // namespace Cir
} // namespace Sta

#endif // STA_UTIL_BACKUP_H
