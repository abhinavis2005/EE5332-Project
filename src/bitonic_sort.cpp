#include "bitonic_sort.h"

void bitonic_sort(index_t padded_n, data_t arr[]) {
  for (int k = 2; k <= padded_n; k *= 2) {
    for (int j = k / 2; j > 0; j /= 2) {
      for (int i = 0; i < padded_n; i++) {
        int l = i ^ j;
        if (l > i) {
          int dir = (i & k) == 0;
          compare_and_swap(arr[i], arr[l], dir);
        }
      }
    }
  }
}
