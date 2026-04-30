#include "bitonic_sort.h"

void compare_and_swap(data_t &a, data_t &b, int dir) {
#pragma HLS INLINE
  if ((a > b) == dir) {
    data_t t = a;
    a = b;
    b = t;
  }
}

constexpr int log2(int m) {
  int r = 0;
  while (m > 1) { m >>= 1; ++r; }
  return r;
}

// index of the lower element of the p-th pair at stride J
constexpr int pair_index(int J, int P) {
  return ((P & ~(J - 1)) << 1) | (P & (J - 1));
}

template <int J, int K, int A>
void cas_pair(data_t in[N], data_t out[N]) {
#pragma HLS INLINE
  constexpr int  B   = A | J;
  constexpr bool dir = (A & K) == 0;
  bool do_swap = (in[A] > in[B]) == dir;
  out[A] = do_swap ? in[B] : in[A];
  out[B] = do_swap ? in[A] : in[B];
}

// expand all N/2 compare-swap pairs for one pass
template <int J, int K, int P = 0>
void pass_step(data_t in[N], data_t out[N]) {
#pragma HLS INLINE
  if constexpr (P < N / 2) {
    cas_pair<J, K, pair_index(J, P)>(in, out);
    pass_step<J, K, P + 1>(in, out);
  }
}

template <int J, int K>
void bitonic_pass(data_t in[N], data_t out[N]) {
#pragma HLS INLINE
  pass_step<J, K>(in, out);
}

// generate all (K, J) passes: K doubles each outer step, J halves each inner step
template <int K, int J, int Step>
void bitonic_gen(data_t stages[][N]) {
#pragma HLS INLINE
  if constexpr (K <= N) {
    bitonic_pass<J, K>(stages[Step], stages[Step + 1]);
    constexpr int NextK = (J > 1) ? K     : K * 2;
    constexpr int NextJ = (J > 1) ? J / 2 : K;     // when J hits 1, next J = new K/2
    bitonic_gen<NextK, NextJ, Step + 1>(stages);
  }
}

void bitonic_sort(data_t arr[N]) {
#pragma HLS INLINE
  constexpr int LogN   = log2(N);
  constexpr int PASSES = LogN * (LogN + 1) / 2;

  data_t stages[PASSES + 1][N];
#pragma HLS ARRAY_PARTITION variable=stages complete dim=2

  for (int i = 0; i < N; i++) {
#pragma HLS UNROLL
    stages[0][i] = arr[i];
  }

  bitonic_gen<2, 1, 0>(stages);

  for (int i = 0; i < N; i++) {
#pragma HLS UNROLL
    arr[i] = stages[PASSES][i];
  }
}
