#include "bitonic_sort.h"

template <int J, int K>
void bitonic_pass(data_t in[N], data_t out[N]) {
#pragma HLS INLINE
  for (int i = 0; i < N; i++) {
#pragma HLS UNROLL 
    int l = i ^ J;
    if (l > i) {
      int dir = (i & K) == 0;
      if ((in[i] > in[l]) == dir) {
        out[i] = in[l];
        out[l] = in[i];
      } else {
        out[i] = in[i];
        out[l] = in[l];
      }
    }
  }
}

void bitonic_sort(data_t arr[N]) {
#pragma HLS INLINE
  data_t s0[N],  s1[N],  s2[N],  s3[N],  s4[N];
  data_t s5[N],  s6[N],  s7[N],  s8[N],  s9[N];
  data_t s10[N], s11[N], s12[N], s13[N], s14[N];
  data_t s15[N], s16[N], s17[N], s18[N], s19[N];
#pragma HLS ARRAY_PARTITION variable=s0  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s1  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s2  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s3  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s4  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s5  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s6  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s7  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s8  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s9  complete dim=1
#pragma HLS ARRAY_PARTITION variable=s10 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s11 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s12 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s13 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s14 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s15 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s16 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s17 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s18 complete dim=1
#pragma HLS ARRAY_PARTITION variable=s19 complete dim=1

  // k = 2
  bitonic_pass<1,  2>(arr, s0);
  // k = 4
  bitonic_pass<2,  4>(s0,  s1);
  bitonic_pass<1,  4>(s1,  s2);
  // k = 8
  bitonic_pass<4,  8>(s2,  s3);
  bitonic_pass<2,  8>(s3,  s4);
  bitonic_pass<1,  8>(s4,  s5);
  // k = 16
  bitonic_pass<8,  16>(s5,  s6);
  bitonic_pass<4,  16>(s6,  s7);
  bitonic_pass<2,  16>(s7,  s8);
  bitonic_pass<1,  16>(s8,  s9);
  // k = 32
  bitonic_pass<16, 32>(s9,  s10);
  bitonic_pass<8,  32>(s10, s11);
  bitonic_pass<4,  32>(s11, s12);
  bitonic_pass<2,  32>(s12, s13);
  bitonic_pass<1,  32>(s13, s14);
  // k = 64
  bitonic_pass<32, 64>(s14, s15);
  bitonic_pass<16, 64>(s15, s16);
  bitonic_pass<8,  64>(s16, s17);
  bitonic_pass<4,  64>(s17, s18);
  bitonic_pass<2,  64>(s18, s19);
  bitonic_pass<1,  64>(s19, arr);
}
