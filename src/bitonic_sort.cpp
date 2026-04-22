#include "bitonic_sort.h"

void bitonic_sort(data_t arr[PADDED_N]) {
  for (int k = 2; k <= PADDED_N; k *= 2) {
    for (int j = k / 2; j > 0; j /= 2) {
      for (int i = 0; i < PADDED_N; i++) {
        int l = i ^ j;
        if (l > i) {
          int dir = (i & k) == 0;
          compare_and_swap(arr[i], arr[l], dir);
        }
      }
    }
  }
}
