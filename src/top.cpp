#include "bitonic_sort.h"
#include <ap_axi_sdata.h>
#include <hls_stream.h>
#include <iostream>

typedef ap_axis<32, 1, 1, 1> axis_pkt_t;

void sort_top(hls::stream<axis_pkt_t> &in_stream,
              hls::stream<axis_pkt_t> &out_stream, ap_uint<8> N) {

  static data_t arr[MAX_N];
  std::fill(arr, arr + MAX_N, SENTINEL);

  const index_t padded_n = 1 << ceil_log2(N);

  axis_pkt_t in_pkt;
  axis_pkt_t out_pkt;

  union {
    data_t data;
    unsigned u;
  } conv;

  // Copy the real N inputs into the first N slots
  for (int i = 0; i < N; i++) {
    in_stream.read(in_pkt);
    conv.u = (unsigned)(ap_uint<32>)in_pkt.data;
    arr[i] = conv.data;
  }

  // Bitonic sort on exactly PADDED_N elements
  bitonic_sort(padded_n, arr);

  // Stream only the first N sorted elements out (sentinels are at the tail)
  for (int i = 0; i < N; i++) {
    conv.data = arr[i];
    out_pkt.data = (ap_int<32>)conv.u;
    out_pkt.keep = -1;
    out_pkt.strb = -1;
    out_pkt.user = 0;
    out_pkt.id = 0;
    out_pkt.dest = 0;
    out_pkt.last = (i == N - 1);
    out_stream.write(out_pkt);
  }
}
