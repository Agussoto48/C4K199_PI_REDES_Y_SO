# Proyecto Cliente-Servidor + Filesystem
# CI0123

CXX      := g++
CXXFLAGS := -g -std=c++11 -pthread -I. -I./filesystem/include
LDFLAGS  := -pthread

BIN := bin

SOCKETS_SRC    := Socket.cc VSocket.cc
FILESYSTEM_SRC := filesystem/src/Bloque.cpp \
                  filesystem/src/Figura.cpp \
                  filesystem/src/SistemaArchivos.cpp

all: $(BIN)/servidor.out $(BIN)/cargar_figuras.out $(BIN)/intermediario.out $(BIN)/cliente.out

$(BIN)/servidor.out: ServidorPiezas.cc $(SOCKETS_SRC) $(FILESYSTEM_SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN)/cargar_figuras.out: filesystem/src/CargarFiguras.cpp $(FILESYSTEM_SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/intermediario.out: intermediario.cc $(SOCKETS_SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN)/cliente.out: cliente.cc $(SOCKETS_SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)

purge: clean
	rm -f lego.dat
