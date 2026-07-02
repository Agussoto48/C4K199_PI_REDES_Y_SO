# Proyecto Intermediario
# CI0123

CXX := g++
CXXFLAGS := -g -std=c++11 -pthread -I .
LDFLAGS := -pthread

BIN := bin

SOCKETS_SRC := Socket.cc VSocket.cc

all: $(BIN)/intermediario.out

$(BIN)/intermediario.out: intermediario.cc $(SOCKETS_SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)