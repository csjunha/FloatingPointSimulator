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

TARGET := $(BIN_DIR)/sim

all: $(TARGET)

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

$(TARGET): $(SIM_OBJ) $(OBJ_FILES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SIM_OBJ): $(SIM) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean