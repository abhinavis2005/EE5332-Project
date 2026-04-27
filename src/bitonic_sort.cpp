#include "bitonic_sort.h"

void compare_and_swap(data_t &a, data_t &b, int dir) {
#pragma HLS INLINE
  if ((a > b) == dir) {
    data_t t = a;
    a = b;
    b = t;
  }
}

void bitonic_sort(data_t arr[N]) {
#pragma HLS INLINE
LOOP_K:
  for (int k = 2; k <= N; k *= 2) {
#pragma HLS PIPELINE II = 1
  LOOP_J:
    for (int j = k / 2; j > 0; j /= 2) {
#pragma HLS PIPELINE II = 1
    LOOP_I:
      for (int i = 0; i < N; i++) {
#pragma HLS UNROLL factor = 16
        int l = i ^ j;
        if (l > i) {
          int dir = (i & k) == 0;
          compare_and_swap(arr[i], arr[l], dir);
        }
      }
    }
  }
}
