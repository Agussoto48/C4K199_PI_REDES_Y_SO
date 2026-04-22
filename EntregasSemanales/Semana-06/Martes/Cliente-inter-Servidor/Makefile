# Proyecto Cliente-Servidor + Filesystem
# CI0123
# Linux Makefile

CXX := g++
CXXFLAGS := -g -std=c++11 -pthread \
	-I./Sockets/include \
	-I./Filesystem/include

LDFLAGS := -pthread

BIN_DIR := bin
DATA_DIR := data

SERVER_TARGET := $(BIN_DIR)/servidor.out
LOADER_TARGET := $(BIN_DIR)/cargar_figuras.out
INTER_TARGET := $(BIN_DIR)/intermediario.out
CLIENT_TARGET := $(BIN_DIR)/cliente.out

SOCKETS_SRC := \
	Sockets/src/Socket.cc \
	Sockets/src/VSocket.cc

FILESYSTEM_SRC := \
	Filesystem/src/Bloque.cpp \
	Filesystem/src/Figura.cpp \
	Filesystem/src/SistemaArchivos.cpp

SERVER_SRC := \
	Servidor/src/ServidorPiezas.cc

LOADER_SRC := \
	Filesystem/src/CargarFiguras.cpp

INTER_SRC := \
	Intermediario/src/intermediario.cc

CLIENT_SRC := \
	Cliente/src/cliente.cc

all: directories $(SERVER_TARGET) $(LOADER_TARGET) $(CLIENT_TARGET) $(INTER_TARGET)

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(DATA_DIR)

$(SERVER_TARGET): $(SERVER_SRC) $(SOCKETS_SRC) $(FILESYSTEM_SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(LOADER_TARGET): $(LOADER_SRC) $(FILESYSTEM_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(INTER_TARGET): $(INTER_SRC) $(SOCKETS_SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(CLIENT_TARGET): $(CLIENT_SRC) $(SOCKETS_SRC)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

server: directories $(SERVER_TARGET)

loader: directories $(LOADER_TARGET)

client: directories $(CLIENT_TARGET)

inter: directories $(INTER_TARGET)

run-loader: $(LOADER_TARGET)
	./$(LOADER_TARGET)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

run-inter: $(INTER_TARGET)
	./$(INTER_TARGET)

clean:
	rm -rf $(BIN_DIR)

purge: clean
	rm -f $(DATA_DIR)/lego.dat