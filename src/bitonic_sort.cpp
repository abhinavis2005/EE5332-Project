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
  for (int k = 2; k <= N; k <<= 1) {

  LOOP_J:
    for (int j = k >> 1; j > 0; j >>= 1) {

    LOOP_P:
      for (int p = 0; p < N / 2; p++) {
#pragma HLS PIPELINE II=1
#pragma HLS UNROLL factor=16

        int low  = p & (j - 1);
        int high = p - low;
        int i    = (high << 1) + low;
        int l    = i + j;

        int dir = ((i & k) == 0);
        compare_and_swap(arr[i], arr[l], dir);
      }
    }
  }
}
