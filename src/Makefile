# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iconfig -Icore

# Directories
SRC_DIR := core
BUILD_DIR := build

# Source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))
MAIN := main.cpp
MAIN_OBJ := $(BUILD_DIR)/main.o

# Output binary
TARGET := sim

# Default target
all: $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $@

# Build the target
$(TARGET): $(MAIN_OBJ) $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile main.cpp
$(MAIN_OBJ): $(MAIN) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile core source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets
.PHONY: all clean