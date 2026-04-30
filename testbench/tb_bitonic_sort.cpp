#include "../src/bitonic_sort.h"
#include <algorithm>
#include <ap_axi_sdata.h>
#include <cstdlib>
#include <hls_stream.h>
#include <iostream>

typedef ap_axis<32, 1, 1, 1> axis_pkt_t;

void sort_top(hls::stream<axis_pkt_t> &in_stream,
              hls::stream<axis_pkt_t> &out_stream,
              ap_uint<1> &flag_post_dma,
              ap_uint<1> &flag_post_sort);

int main() {
  data_t in[N];

  for (int i = 0; i < N; i++)
    in[i] = rand() % 1000;

  hls::stream<axis_pkt_t> in_stream("in");
  hls::stream<axis_pkt_t> out_stream("out");

  for (int i = 0; i < N; i++) {
    axis_pkt_t pkt;
    pkt.data = (ap_int<32>)(ap_uint<32>)in[i];
    pkt.keep = -1;
    pkt.strb = -1;
    pkt.user = 0;
    pkt.id = 0;
    pkt.dest = 0;
    pkt.last = (i == N - 1) ? 1 : 0;
    in_stream.write(pkt);
  }

  ap_uint<1> flag_post_dma, flag_post_sort;
  sort_top(in_stream, out_stream, flag_post_dma, flag_post_sort);

  data_t out[N];
  for (int i = 0; i < N; i++) {
    axis_pkt_t pkt = out_stream.read();
    out[i] = (data_t)(ap_uint<32>)pkt.data;
  }

  data_t expected[N];
  for (int i = 0; i < N; i++)
    expected[i] = in[i];
  std::sort(expected, expected + N);

  bool pass = true;
  for (int i = 0; i < N; i++) {
    if (out[i] != expected[i]) {
      pass = false;
      break;
    }
  }

  std::cout << "Input:    ";
  for (int i = 0; i < N; i++) std::cout << in[i] << " ";
  std::cout << "\nOutput:   ";
  for (int i = 0; i < N; i++) std::cout << out[i] << " ";
  std::cout << "\nExpected: ";
  for (int i = 0; i < N; i++) std::cout << expected[i] << " ";
  std::cout << "\n";

  if (pass)
    std::cout << "\033[32mPASS\033[0m\n";
  else
    std::cout << "\033[31mFAIL\033[0m\n";

    if (pass)
        std::cout << "PASS" << std::endl;
    else
        std::cout << "\033[31mFAIL\033[0m" << std::endl;
    return pass ? 0 : 1;
}
