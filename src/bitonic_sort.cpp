#include "bitonic_sort.h"

constexpr int pair_index(int J, int P){
  return ((P & ~(J-1)) << 1 ) | (P & (J-1));
}

template <int J, int K, int A>
void cas_pair(data_t in[N], data_t out[N]){
#pragma HLS INLINE
  constexpr int B = A | J;
  constexpr bool dir = (A & K) == 0;
  data_t a = in[A], b = in[B];
  bool do_swap = (a > b) == dir;
  out[A] = do_swap ? b : a;
  out[B] = do_swap ? a : b;
}

template<int J, int K, int P = 0>
void pass_step(data_t in[N], data_t out[N]){
#pragma HLS INLINE
  if constexpr (P < N/2){
    cas_pair<J, K, pair_index(J, P)>(in, out);
    pass_step<J, K, P+1>(in, out);
  }
}

template <int K, int J>
void bitnoic_gen(data_t arr[N]){
#pragma HLS INLINE
  if constexpr (K <= N){
    pass_step<J, K>(arr, arr);
    constexpr int NextK = ( J > 1) ? K : K*2;
    constexpr int NextJ = (J > 1) ? J/2 : K;
    bitnoic_gen<NextK, NextJ>(arr);
  }
}

void bitonic_sort(data_t arr[N]) {
#pragma HLS PIPELINE OFF
  bitnoic_gen<2, 1>(arr);
}
