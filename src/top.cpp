#include "bitonic_sort.h"
#include <ap_axi_sdata.h>
#include <hls_stream.h>

typedef ap_axis<32, 1, 1, 1> axis_pkt_t;

void sort_top(hls::stream<axis_pkt_t> &in_stream,
              hls::stream<axis_pkt_t> &out_stream,
              ap_uint<1> &flag_post_dma,
              ap_uint<1> &flag_post_sort) {
#pragma HLS INTERFACE axis     port = in_stream
#pragma HLS INTERFACE axis     port = out_stream
#pragma HLS INTERFACE s_axilite port = flag_post_dma  bundle = control
#pragma HLS INTERFACE s_axilite port = flag_post_sort bundle = control
#pragma HLS INTERFACE s_axilite port = return         bundle = control

  data_t arr[N];

  flag_post_dma  = 0;
  flag_post_sort = 0;

  axis_pkt_t pkt;

INPUT_LOOP:
  for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II = 1
    in_stream.read(pkt);
    arr[i] = (data_t)(ap_uint<32>)pkt.data;
  }
  flag_post_dma = 1;

  bitonic_sort(arr);
  flag_post_sort = 1;

OUTPUT_LOOP:
  for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II = 1
    pkt.data = (ap_int<32>)(ap_uint<32>)arr[i];
    pkt.keep = -1;
    pkt.strb = -1;
    pkt.user = 0;
    pkt.id = 0;
    pkt.dest = 0;
    pkt.last = (i == N - 1);
    out_stream.write(pkt);
  }
}
