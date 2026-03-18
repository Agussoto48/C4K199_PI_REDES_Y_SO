CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

BIN_DIR = bin

SRC_IPv4 = src/IPv4.cpp src/Socket.cpp src/VSocket.cpp
SRC_IPv6 = src/IPv6.cpp src/Socket.cpp src/VSocket.cpp
SRC_SIM = src/sim.cpp 

EXE_IPv4 = $(BIN_DIR)/IPv4
EXE_IPv6 = $(BIN_DIR)/IPv6
EXE_SIM = $(BIN_DIR)/Sim

default: build_IPv4 build_IPv6 build_Sim

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)


build_IPv4: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_IPv4) -o $(EXE_IPv4)

run_IPv4:
	@./$(EXE_IPv4)

build_IPv6: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_IPv6) -o $(EXE_IPv6)

run_IPv6:
	@./$(EXE_IPv6)

build_Sim: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_SIM) -o $(EXE_SIM)

run_Sim:
	@./$(EXE_SIM)

clean:
	@rm -rf $(BIN_DIR)
