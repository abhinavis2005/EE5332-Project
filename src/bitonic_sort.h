#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

typedef int data_t;

static const int N = 256;

void compare_and_swap(data_t &a, data_t &b, int dir);
void bitonic_sort(data_t arr[N]);

#endif
