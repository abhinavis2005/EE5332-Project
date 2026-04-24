#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include "ap_int.h"
#include <cstddef>

typedef int data_t;

constexpr size_t N_BITS = 8; // number of bits in the index
typedef ap_uint<N_BITS> index_t;
constexpr size_t MAX_N = (1 << N_BITS) - 1;

constexpr data_t SENTINEL{0x7FFFFFFF};

// Priority encoder: ceil(log2(N)) for N up to 256
// i.e., smallest k such that 2^k >= N
inline index_t ceil_log2(index_t n) {
  if (n <= 1)
    return 0;
  int k = 0;
  int val = 1;
  while (val < n) {
    val <<= 1;
    k++;
  }
  return k;
}

inline void compare_and_swap(data_t &a, data_t &b, int dir) {
  if ((a > b) == dir) {
    data_t t = a;
    a = b;
    b = t;
  }
}

void bitonic_sort(index_t padded_n, data_t arr[]);

#endif
