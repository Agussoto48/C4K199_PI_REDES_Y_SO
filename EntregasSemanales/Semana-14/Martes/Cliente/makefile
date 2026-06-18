CXX := g++
CXXFLAGS := -g -std=c++11 -pthread -Iinclude

BIN := bin
TARGET := $(BIN)/cliente.out

SRC := src/main.cpp src/Cliente.cpp

all: $(TARGET)

$(TARGET): $(SRC) | $(BIN)
	$(CXX) $(CXXFLAGS) $(SRC) -o $@

$(BIN):
	mkdir -p $(BIN)

run: all
	./$(TARGET)

clean:
	rm -rf $(BIN)