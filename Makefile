CXX      := g++
CXXFLAGS := -std=c++14 -O2 -I src

SRC  := src/bitonic_sort.cpp
TB   := testbench/tb_bitonic_sort.cpp
BIN  := sim_bitonic

.PHONY: sim clean

sim: $(BIN)
	./$(BIN)

$(BIN): $(SRC) $(TB)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(BIN)
