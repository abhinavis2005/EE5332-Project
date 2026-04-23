#include "../src/bitonic_sort.h"
#include <cstdlib>
#include <iostream>

void sort_top(data_t in_stream[N], data_t out_stream[N]);

int main() {
  data_t in[N], out[N];

  for (int i = 0; i < N; i++)
    in[i] = rand() % 100;

  sort_top(in, out);

  int pass = 1;
  for (int i = 0; i < N - 1; i++) {
    if (out[i] > out[i + 1]) {
      pass = 0;
      break;
    }
  }

  if (pass)
    std::cout << "PASS" << std::endl;
  else
    std::cout << "\033[31mFAIL\033[0m" << std::endl;
  return pass ? 0 : 1;
}
