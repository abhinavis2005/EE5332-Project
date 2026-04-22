#include "bitonic_sort.h"

void sort_top(data_t in_stream[N], data_t out_stream[N]) {

  data_t arr[PADDED_N];

  // Copy the real N inputs into the first N slots
  for (int i = 0; i < N; i++) {
    arr[i] = in_stream[i];
  }

  // If N is not a power of 2, pad slots [N .. PADDED_N-1] with SENTINEL
  // If N is a power of 2, PADDED_N == N so this loop body never executes
  for (int i = N; i < PADDED_N; i++) {
    arr[i] = SENTINEL;
  }

  // Bitonic sort on exactly PADDED_N elements
  bitonic_sort(arr);

  // Stream only the first N sorted elements out (sentinels are at the tail)
  for (int i = 0; i < N; i++) {
    out_stream[i] = arr[i];
  }
}
