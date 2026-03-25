CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -I../shared/include
LDFLAGS = -lssl -lcrypto

BIN_DIR = bin
SHARED_CODE = ../shared/src/Socket.cpp ../shared/src/VSocket.cpp

SRC_IPV4_SSL = src/ipv4-ssl-cli.cpp src/SSLSocket.cpp $(SHARED_CODE)
SRC_EXAMPLE = src/ssl-cli.cpp

EXE_IPV4_SSL = $(BIN_DIR)/ipv4-ssl-cli
EXE_EXAMPLE = $(BIN_DIR)/ssl-cli


default: build_SSL_IPv4 build_example

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

build_example: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_EXAMPLE) -o $(EXE_EXAMPLE) $(LDFLAGS)

run_example:
	@./$(EXE_EXAMPLE) $(ARGS)

build_SSL_IPv4: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_IPV4_SSL) -o $(EXE_IPV4_SSL) $(LDFLAGS)

run_SSL_IPv4:
	@./$(EXE_IPV4_SSL)

clean:
	@rm -rf $(BIN_DIR)
