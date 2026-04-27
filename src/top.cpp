#include "bitonic_sort.h"
#include <ap_axi_sdata.h>
#include <hls_stream.h>

typedef ap_axis<32, 1, 1, 1> axis_pkt_t;

void sort_top(hls::stream<axis_pkt_t> &in_stream,
              hls::stream<axis_pkt_t> &out_stream) {
#pragma HLS INTERFACE axis port = in_stream
#pragma HLS INTERFACE axis port = out_stream
#pragma HLS INTERFACE s_axilite port = return bundle = control

  data_t arr[N];
#pragma HLS ARRAY_PARTITION variable = arr cyclic factor = 16 dim = 1

  union {
    data_t d;
    unsigned u;
  } conv;

  axis_pkt_t pkt;

INPUT_LOOP:
  for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II = 1
    in_stream.read(pkt);
    conv.u = (unsigned)(ap_uint<32>)pkt.data;
    arr[i] = conv.d;
  }

  bitonic_sort(arr);

OUTPUT_LOOP:
  for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II = 1
    conv.d = arr[i];
    pkt.data = (ap_int<32>)conv.u;
    pkt.keep = -1;
    pkt.strb = -1;
    pkt.user = 0;
    pkt.id = 0;
    pkt.dest = 0;
    pkt.last = (i == N - 1);
    out_stream.write(pkt);
  }
}
