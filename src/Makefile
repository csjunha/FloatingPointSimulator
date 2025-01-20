# Boost
BOOST_LIB_PATH = /usr/local/lib64
BOOST_LIBS = -lboost_program_options

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iconfig -Icore
LDFLAGS := -L$(BOOST_LIB_PATH) $(BOOST_LIBS)

BIN_DIR := bin
SRC_DIR := core
BUILD_DIR := build

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

SIM := sim.cpp
GEN := gen.cpp

SIM_OBJ := $(BUILD_DIR)/sim.o
GEN_OBJ := $(BUILD_DIR)/gen.o

SIM_EXEC = $(BIN_DIR)/sim
GEN_EXEC = $(BIN_DIR)/gen

all: $(SIM_EXEC) $(GEN_EXEC)

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

$(SIM_EXEC): $(SIM_OBJ) $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SIM_OBJ): $(SIM) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GEN_EXEC): $(GEN_OBJ) $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(GEN_OBJ): $(GEN) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean