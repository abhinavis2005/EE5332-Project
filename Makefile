CXX      := g++
XILINX_HLS ?= /home/abhinav/Documents/vivado/Vitis_HLS/2021.1
HLS_INC  := -I$(XILINX_HLS)/include
CXXFLAGS := -std=c++14 -O2 -I src $(HLS_INC)

SRC  := src/bitonic_sort.cpp src/top.cpp
TB   := testbench/tb_bitonic_sort.cpp
BIN  := sim_bitonic

.PHONY: sim synth clean

sim: $(BIN)
	./$(BIN)

$(BIN): $(SRC) $(TB)
	$(CXX) $(CXXFLAGS) $^ -o $@

synth:
	cd hls && vitis_hls -f synth.tcl

clean:
	rm -f $(BIN)
