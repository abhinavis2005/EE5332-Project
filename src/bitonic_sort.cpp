#include "bitonic_sort.h"

void compare_and_swap(data_t &a, data_t &b, int dir) {
#pragma HLS PIPELINE II = 1
  if ((a > b) == dir) {
    data_t t = a;
    a = b;
    b = t;
  }
}

void bitonic_sort(index_t padded_n, data_t arr[]) {
#pragma HLS INLINE
LOOP1:
  for (int k = 2; k <= padded_n; k *= 2) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = 8 avg = 4
  LOOP2:
    for (int j = k / 2; j > 0; j /= 2) {
#pragma HLS LOOP_TRIPCOUNT min = 0 max = 8 avg = 4
    LOOP3:
      for (int i = 0; i < MAX_N; i++) {
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min = 2 max = 256 avg = 128
        int l = i ^ j;
        if (l > i) {
          int dir = (i & k) == 0;
          compare_and_swap(arr[i], arr[l], dir);
        }
      }
    }
  }
}
