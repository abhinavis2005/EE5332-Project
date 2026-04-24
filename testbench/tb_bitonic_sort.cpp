#include "../src/bitonic_sort.h"
#include <algorithm>
#include <ap_axi_sdata.h>
#include <cstdlib>
#include <hls_stream.h>
#include <iostream>

typedef ap_axis<32, 1, 1, 1> axis_pkt_t;

void sort_top(hls::stream<axis_pkt_t> &in_stream,
              hls::stream<axis_pkt_t> &out_stream, ap_uint<8> N);

int main() {
  const ap_uint<8> N = 8;
  data_t in[MAX_N];

  for (int i = 0; i < N; i++)
    in[i] = rand() % 100;

  hls::stream<axis_pkt_t> in_stream("in");
  hls::stream<axis_pkt_t> out_stream("out");

  union {
    data_t data;
    unsigned u;
  } conv;

  for (int i = 0; i < N; i++) {
    axis_pkt_t pkt;
    conv.data = in[i];
    pkt.data = (ap_int<32>)conv.u;
    pkt.keep = -1;
    pkt.strb = -1;
    pkt.user = 0;
    pkt.id = 0;
    pkt.dest = 0;
    pkt.last = (i == N - 1) ? 1 : 0;
    in_stream.write(pkt);
  }

  sort_top(in_stream, out_stream, N);

  data_t out[MAX_N];
  for (int i = 0; i < N; i++) {
    axis_pkt_t pkt = out_stream.read();
    conv.u = (unsigned)(ap_uint<32>)pkt.data;
    out[i] = conv.data;
  }

  data_t expected[MAX_N];
  for (int i = 0; i < N; i++)
    expected[i] = in[i];
  std::sort(expected, expected + N);

  int pass = 1;
  for (int i = 0; i < N; i++) {
    if (out[i] != expected[i]) {
      pass = 0;
      break;
    }
  }

  std::cout << "Input:  ";
  for (int i = 0; i < N; i++)
    std::cout << in[i] << " ";
  std::cout << std::endl;
  std::cout << "Output: ";
  for (int i = 0; i < N; i++)
    std::cout << out[i] << " ";
  std::cout << std::endl;

  if (pass) {
    std::cout << "\033[32mPASS\033[0m" << std::endl;
  } else {
    std::cout << "\033[31mFAIL\033[0m" << std::endl;
  }
  return pass ? 0 : 1;
}
