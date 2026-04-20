#include <iostream>
#include <cstdlib>
#include "../src/bitonic_sort.h"

int main() {
    data_t arr[N];
    data_t ref[N];

    for (int i = 0; i < N; i++) {
        arr[i] = ref[i] = rand() % 100;
    }

    bitonic_sort(arr);

    // simple check: verify sorted order
    int pass = 1;
    for (int i = 0; i < N - 1; i++) {
        if (arr[i] > arr[i + 1]) { pass = 0; break; }
    }

    if (pass)
        std::cout << "PASS" << std::endl;
    else
        std::cout << "\033[31mFAIL\033[0m" << std::endl;
    return pass ? 0 : 1;
}
