CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -pthread

BIN_DIR = bin


SRC_SIM = src/*.cpp 

EXE_SIM = $(BIN_DIR)/Sim


default: build_Sim 

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)



build_Sim: | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(SRC_SIM) -o $(EXE_SIM)

run_Sim:
	@./$(EXE_SIM)


clean:
	@rm -rf $(BIN_DIR)
