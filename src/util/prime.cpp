#include "sta/src/util/prime.h"

size_t Sta::Util::prime_gt(size_t n) {
    const size_t arr[] = {
        0, 2, 5, 11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421,
        12853, 25717, 51437, 102877, 205759, 411527, 823117,
        1646237, 3292489, 6584983, 13169977, 26339969, 52679969,
        105359939, 210719981, 421439981, 842879963, 1685759983,
        3371519971, 6743039959, 13486079921, 26972159857};

    int beg = 0;
    int end = 35;

    while (beg != end - 1) {
        int mid = (beg + end)/2;

        if (arr[mid] >= n) {
            end = mid;
        }
        else {
            beg = mid;
        }
    }

    return arr[end];
}

