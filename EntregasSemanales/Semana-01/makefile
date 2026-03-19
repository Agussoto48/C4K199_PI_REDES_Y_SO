CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude

BIN_DIR = bin

SRC_IPv4 = src/IPv4.cpp src/Socket.cpp src/VSocket.cpp
SRC_IPv6 = src/IPv6.cpp src/Socket.cpp src/VSocket.cpp
SRC_SIM = src/sim.cpp 
SRC_CLIENTE_IPV4 = src/ipv4-udp-client.cpp src/Socket.cpp src/VSocket.cpp
SRC_SERVER_IPV4 = src/ipv4-udp-server.cpp src/Socket.cpp src/VSocket.cpp
SRC_CLIENTE_IPV6 = src/ipv6-udp-client.cpp src/Socket.cpp src/VSocket.cpp
SRC_SERVER_IPV6 = src/ipv6-udp-server.cpp src/Socket.cpp src/VSocket.cpp

EXE_IPv4 = $(BIN_DIR)/IPv4
EXE_IPv6 = $(BIN_DIR)/IPv6
EXE_SIM = $(BIN_DIR)/Sim
EXE_CLIENTE_IPV4 = $(BIN_DIR)/cliente_IPV4
EXE_SERVER_IPV4 = $(BIN_DIR)/server_IPV4
EXE_CLIENTE_IPV6 = $(BIN_DIR)/cliente_IPV6
EXE_SERVER_IPV6 = $(BIN_DIR)/server_IPV6

default: build_IPv4 build_IPv6 build_Sim build_Cliente_IPV4 build_Server_IPV4 build_Cliente_IPV6 build_Server_IPV6

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

build_Cliente_IPV4: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_CLIENTE_IPV4) -o $(EXE_CLIENTE_IPV4)

run_Cliente_IPV4:
	@./$(EXE_CLIENTE_IPV4)

build_Server_IPV4: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_SERVER_IPV4) -o $(EXE_SERVER_IPV4)

run_Server_IPV4:
	@./$(EXE_SERVER_IPV4)

build_Cliente_IPV6: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_CLIENTE_IPV6) -o $(EXE_CLIENTE_IPV6)

run_Cliente_IPV6:
	@./$(EXE_CLIENTE_IPV6)

build_Server_IPV6: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_SERVER_IPV6) -o $(EXE_SERVER_IPV6)

run_Server_IPV6:
	@./$(EXE_SERVER_IPV6)

clean:
	@rm -rf $(BIN_DIR)
