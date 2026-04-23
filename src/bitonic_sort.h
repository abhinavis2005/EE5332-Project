#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include "ap_int.h"
#include <cstddef>

typedef int data_t;

constexpr size_t N_BITS = 8; // number of bits in the index
typedef ap_uint<N_BITS> index_t;
constexpr size_t MAX_N = (1 << N_BITS) - 1;

#define SENTINEL 0x7FFFFFFF

// Check if N is a power of 2
#define IS_POWER_OF_2 ((N & (N - 1)) == 0)

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
#define CEIL_LOG2_N                                                            \
  ((N <= 1)     ? 0                                                            \
   : (N <= 2)   ? 1                                                            \
   : (N <= 4)   ? 2                                                            \
   : (N <= 8)   ? 3                                                            \
   : (N <= 16)  ? 4                                                            \
   : (N <= 32)  ? 5                                                            \
   : (N <= 64)  ? 6                                                            \
   : (N <= 128) ? 7                                                            \
                : 8)

// The padded size: 2^k where k = ceil(log2(N))
// If N is already a power of 2, this equals N exactly
#define PADDED_N (1 << CEIL_LOG2_N)

inline void compare_and_swap(data_t &a, data_t &b, int dir) {
  if ((a > b) == dir) {
    data_t t = a;
    a = b;
    b = t;
  }
}

void bitonic_sort(index_t padded_n, data_t arr[]);

#endif
