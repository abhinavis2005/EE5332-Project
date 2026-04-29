#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <ap_int.h>

typedef ap_uint<16> data_t;

static const int N = 64;

void compare_and_swap(data_t &a, data_t &b, int dir);
void bitonic_sort(data_t arr[N]);

#endif
