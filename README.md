# EE5332 Project — Bitonic Sort on FPGA (Vitis HLS)

Hardware implementation of a **64-element Bitonic Sort** targeting the Zynq xc7z020 FPGA,
developed as part of EE5332. The project explores four HLS design strategies, each tracked
on its own branch, with a progression from a naive baseline to a resource-optimal systolic
architecture.

---

## Algorithm

Bitonic sort is a **sorting network**: every comparison is decided at compile time, so
the same sequence of compare-and-swap (CAS) operations executes regardless of input.

For N = 64 there are log₂(64) = 6 merge stages, subdivided into sub-passes:

```
1 + 2 + 3 + 4 + 5 + 6 = 21 passes total
```

Each pass performs N/2 = 32 independent CAS operations on pairs selected by XOR with a
fixed stride J, with sort direction controlled by a mask K.

---

## Interface

All designs share the same AXI-Stream top-level interface targeting PS↔PL integration
on the PYNQ-Z2:

```
in_stream  (AXI-Stream, 32-bit words, carries ap_uint<16> data in lower half)
out_stream (AXI-Stream, 32-bit words)
flag_post_dma  (AXI-Lite read, set after DMA input completes)
flag_post_sort (AXI-Lite read, set after sort completes)
```

The two AXI-Lite flags allow the PS to measure sort latency cycle-accurately without
touching the data path.

---

## Branches

### `basic-n-64` — Baseline nested loops

The most direct HLS translation of the textbook algorithm: three nested loops over k, j,
and i, performing in-place compare-and-swaps on a single `arr[N]` array.

**Key characteristics:**
- `#pragma HLS PIPELINE OFF` — no loop unrolling or pipelining inside the sort
- `#pragma HLS ARRAY_PARTITION complete` on `arr` — all elements are registers, but
  comparisons are still serialised by the loops
- The variable inner-loop bound (`j /= 2`) prevents HLS from statically scheduling the
  sort, leading to dynamic token-based arbitration

**Result:** Passes timing comfortably but is slow — the sort kernel iterates ~128 cycles
per innermost loop trip with no parallelism.

| Resource | Used | Available | % |
|----------|------|-----------|---|
| FF | 4,373 | 106,400 | 4% |
| LUT | 36,749 | 53,200 | 69% |
| Timing slack | +3.09 ns | — | PASS |
| Sort latency | — | — | unbounded (loop-dependent) |

---

### `unroll-n-64-loop` — Pragma-based loop unrolling

Adds `#pragma HLS UNROLL` to the innermost loop of the baseline, attempting to execute
all 64 CAS operations of each pass in parallel.

**Key characteristics:**
- Same loop structure as `basic-n-64`
- Inner loop body fully replicated in hardware by HLS unroller
- HLS attempts to instantiate 64 simultaneous comparators across 21 passes

**Result:** Catastrophic LUT overflow. Unrolling 64 comparators per pass without
structural sharing generates ~624K LUT equivalents — 11× the device capacity. The design
also violates timing (-2.73 ns slack). **Not implementable.**

| Resource | Used | Available | % |
|----------|------|-----------|---|
| FF | 39,941 | 106,400 | 37% |
| LUT | 624,298 | 53,200 | **1173%** |
| Timing slack | -2.73 ns | — | **FAIL** |

> **Takeaway:** Blindly applying `#pragma HLS UNROLL` to a data-dependent loop gives HLS
> no structural information — it replicates the full loop body without sharing logic
> across passes, flooding the device.

---

### `unroll-n-64-template` — C++ template metaprogramming

Replaces the runtime loops with compile-time C++ templates. Each pass and each CAS pair
within a pass is a separate template instantiation, fully visible to HLS as static logic.

**Key characteristics:**
- `constexpr pair_index<J,P>()` computes array indices at compile time
- `cas_pair<J,K,A>` — single CAS unit, `#pragma HLS INLINE`
- `pass_step<J,K,P>` — recursive template expanding all N/2 pairs of one pass
- `bitonic_gen<K,J>` — recursive template generating all 21 passes in sequence
- In-place on `arr[N]` with `ARRAY_PARTITION complete`
- No runtime loops visible to HLS — the schedule is entirely static

**Result:** HLS can now see the full combinational graph. Logic sharing across passes
drops LUT usage from 624K (loop unroll) to 48K. Sort kernel completes in 6 cycles.
Timing passes at 1.25 ns slack.

| Resource | Used | Available | % |
|----------|------|-----------|---|
| FF | 8,296 | 106,400 | 7% |
| LUT | 48,279 | 53,200 | 90% |
| Timing slack | +1.25 ns | — | PASS |
| Sort latency | 6 cycles | — | — |
| Total latency | 145 cycles | — | (incl. DMA) |

> **Takeaway:** Template metaprogramming exposes the entire computation graph at compile
> time. HLS can share logic across instantiations, reducing LUT usage by ~13×
> vs. the pragma-unrolled version.

---

### `fixed-n-64-systolic` — Out-of-place systolic pipeline *(merged into `main`)*

Extends the template approach with an explicit **out-of-place** pipeline structure: each
of the 21 passes reads from a dedicated input buffer and writes to a dedicated output
buffer. This gives HLS a clean directed acyclic graph with no read/write aliasing.

**Key characteristics:**
- `stages[PASSES+1][N]` — 22 register banks (one per pass boundary); `PASSES = 21` for N = 64
- `ARRAY_PARTITION complete dim=2` — every element of every stage is a flip-flop, no
  memory ports
- Same `cas_pair` / `pass_step` / `bitonic_gen` template hierarchy as `unroll-n-64-template`
- Aliasing is eliminated: each pass reads `stages[s]` and writes `stages[s+1]`; HLS sees
  no hazards and generates a clean systolic datapath
- `#pragma HLS UNROLL` on init/finalize loops that copy stream data into/out of `stages`

**Result:** Eliminating aliasing allows HLS to share logic more aggressively. LUT usage
drops from 90% (template in-place) to 57%, with essentially the same timing and latency.
This is the best-performing design on this device.

| Resource | Used | Available | % |
|----------|------|-----------|---|
| FF | 9,317 | 106,400 | 8% |
| LUT | 30,782 | 53,200 | **57%** |
| Timing slack | +1.25 ns | — | PASS |
| Sort latency | — | — | — |
| Total latency | 144 cycles | — | (incl. DMA) |

> **Takeaway:** Out-of-place staging eliminates read/write aliasing and gives HLS a clean
> dataflow graph. Combined with template-static scheduling, this cuts LUT usage by ~40%
> relative to in-place templates, at the cost of 21× N × 16 bits of extra flip-flop storage.

---

## Design Comparison

| Branch | Strategy | FF | LUT | Timing | Total latency |
|--------|----------|----|-----|--------|---------------|
| `basic-n-64` | Nested loops, no unroll | 4,373 (4%) | 36,749 (69%) | +3.09 ns ✓ | loop-dependent |
| `unroll-n-64-loop` | Loops + `#pragma UNROLL` | 39,941 (37%) | 624,298 (**1173%**) | -2.73 ns ✗ | N/A |
| `unroll-n-64-template` | Template metaprogramming, in-place | 8,296 (7%) | 48,279 (90%) | +1.25 ns ✓ | 145 cycles |
| `fixed-n-64-systolic` | Templates + out-of-place stages | 9,317 (8%) | 30,782 (**57%**) | +1.25 ns ✓ | 144 cycles |

Device: Zynq xc7z020-clg400-1 (53,200 LUT / 106,400 FF), 15 ns clock target.

---

## Repository Structure

```
src/
  bitonic_sort.h        data_t, N, function declarations
  bitonic_sort.cpp      sort kernel (implementation varies per branch)
  top.cpp               AXI-Stream wrapper + timing flags
testbench/
  tb_bitonic_sort.cpp   software simulation testbench
hls/
  synth.tcl             Vitis HLS synthesis script
  hls_bitonic_sort/     HLS project output (sol1/syn/report/csynth.rpt)
pynq/
  bitonic_sort_test.ipynb  PYNQ-Z2 host-side test notebook
vivado/
  create_project.tcl    Vivado block design script
Makefile                make sim / make synth SOL=<name>
ALGORITHM.md            detailed algorithm and HLS rationale notes
```

---

## Quick Start

**Simulate (software model):**
```bash
make sim
```
Compiles kernel + testbench, runs the binary, prints `PASS` or `FAIL`.

**HLS synthesis:**
```bash
make synth SOL=sol1
```
Runs Vitis HLS and writes reports to `hls/hls_bitonic_sort/sol1/syn/report/`.
