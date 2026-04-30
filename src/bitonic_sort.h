#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <ap_int.h>
constexpr int N = 64;

typedef ap_uint<16> data_t;

inline void compare_and_swap(data_t &a, data_t &b, int dir) {
    if ((a > b) == dir) { data_t t = a; a = b; b = t; }
}

void bitonic_sort(data_t arr[N]);

#endif
