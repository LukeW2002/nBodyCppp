# Compiler
CXX := g++

# Compiler flags
CXXFLAGS :=  -std=c++17 -Wall -march=native -O3 -Wextra -pedantic -I./include -fopenmp

# Linker flags
LDFLAGS := -lSDL2 -fopenmp

# Directories
SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
TEST_DIR := test

# Source files
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TEST_OBJECTS := $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable names
EXECUTABLE := nbody_simulation
TEST_EXECUTABLE := run_tests

# Default target
all: $(EXECUTABLE)

# Rule to create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile test files into object files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to link object files into main executable
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Rule to link object files into test executable
$(TEST_EXECUTABLE): $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS) -lgtest -lgtest_main -pthread

# Test target
test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(TEST_EXECUTABLE) NbodySim.log

# Phony targets
.PHONY: all clean test

# Include dependencies
-include $(OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)

# Rule to generate dependency files
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@

$(BUILD_DIR)/%.d: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:.d=.o) $< > $@
