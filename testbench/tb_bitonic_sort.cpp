#include "../src/bitonic_sort.h"
#include <cstdlib>
#include <iostream>

void sort_top(ap_uint<8> N, data_t in_stream[], data_t out_stream[]);

int main() {
  const ap_uint<8> N = 20;
  data_t in[MAX_N], out[MAX_N];

  for (int i = 0; i < N; i++)
    in[i] = rand() % 100;

  sort_top(N, in, out);

  int pass = 1;
  for (int i = 0; i < N - 1; i++) {
    if (out[i] > out[i + 1]) {
      pass = 0;
      break;
    }
  }

  if (pass) {
    std::cout << "PASS" << std::endl;
  } else {
    std::cout << "\033[31mFAIL\033[0m" << std::endl;
    std::cout << "Input:  ";
    for (int i = 0; i < N; i++) std::cout << in[i] << " ";
    std::cout << std::endl;
    std::cout << "Output: ";
    for (int i = 0; i < N; i++) std::cout << out[i] << " ";
    std::cout << std::endl;
  }
  return pass ? 0 : 1;
}
