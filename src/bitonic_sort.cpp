#include "bitonic_sort.h"

void bitonic_sort(data_t arr[N]) {
#pragma HLS PIPELINE OFF
#pragma HLS ALLOCATION operation=icmp limit=1
    for (int k = 2; k <= N; k *= 2) {
        for (int j = k / 2; j > 0; j /= 2) {
            for (int i = 0; i < N; i++) {
                int l = i ^ j;
                if (l > i) {
                    int dir = (i & k) == 0;
                    compare_and_swap(arr[i], arr[l], dir);
                }
            }
        }
    }
}
