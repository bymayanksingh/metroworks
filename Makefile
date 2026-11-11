CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O3
INCLUDES = -Iinclude

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

LIB_SRCS = $(SRC_DIR)/utils.cpp \
           $(SRC_DIR)/station.cpp \
           $(SRC_DIR)/fare_calculator.cpp \
           $(SRC_DIR)/metro_network.cpp \
           $(SRC_DIR)/data_loader.cpp \
           $(SRC_DIR)/route_planner.cpp \
           $(SRC_DIR)/dot_exporter.cpp

LIB_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SRCS))

CLI_SRC = $(SRC_DIR)/main.cpp
CLI_OBJ = $(BUILD_DIR)/main.o
CLI_BIN = $(BIN_DIR)/metroworks

TEST_SRCS = $(TEST_DIR)/test_main.cpp \
            $(TEST_DIR)/test_graph.cpp \
            $(TEST_DIR)/test_catalog.cpp \
            $(TEST_DIR)/test_fare.cpp \
            $(TEST_DIR)/test_data_loader.cpp \
            $(TEST_DIR)/test_route_planner.cpp \
            $(TEST_DIR)/test_dot_exporter.cpp

TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SRCS))
TEST_BIN = $(BIN_DIR)/metroworks_test

.PHONY: all cli test run_test clean

all: cli test

cli: $(CLI_BIN)

test: $(TEST_BIN)
	@echo "Running test suite..."
	@./$(TEST_BIN)

$(CLI_BIN): $(LIB_OBJS) $(CLI_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(TEST_DIR) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)/tests
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I$(TEST_DIR) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/tests:
	mkdir -p $(BUILD_DIR)/tests

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
