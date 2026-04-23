CXX      := g++
CXXFLAGS := -std=c++14 -O2 -I src

SRC  := src/bitonic_sort.cpp src/top.cpp
TB   := testbench/tb_bitonic_sort.cpp
BIN  := sim_bitonic

.PHONY: sim synth clean

sim: $(BIN)
	./$(BIN)

$(BIN): $(SRC) $(TB)
	$(CXX) $(CXXFLAGS) $^ -o $@

synth:
	./vivado_run.sh vitis_hls -f hls/synth.tcl

clean:
	rm -f $(BIN)
