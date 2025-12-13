CC = g++-13

NPROCS := $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu)
MAKEFLAGS += --jobs=$(NPROCS)

CFLAGS   = -Wall -O2 -std=c++20
LDFLAGS  = -lncurses -pthread
DEPFLAGS = -MMD -MP

SRC_DIR     = src
UTIL_DIR    = $(SRC_DIR)/util
KEYBINDS_DIR= $(SRC_DIR)/keybinds
FUZZ_DIR    = $(SRC_DIR)/fuzzer
TEST_DIR    = test
TEST_UTIL_DIR    = test/util
BUILD_DIR   = build


UTIL_SRCS := $(wildcard $(UTIL_DIR)/*.cpp)
KEYBINDS_SRCS := $(wildcard $(KEYBINDS_DIR)/*.cpp)

TEST_FILE_SRCS := $(wildcard $(TEST_UTIL_DIR)/*.cpp)

COMMON_SRCS := $(UTIL_SRCS) $(KEYBINDS_SRCS)
MAIN_SRCS = $(SRC_DIR)/edit.cpp $(COMMON_SRCS)
FUZZ_SRCS = $(FUZZ_DIR)/fuzzer.cpp $(COMMON_SRCS)
TEST_SRCS = $(TEST_DIR)/test.cpp $(TEST_FILE_SRCS) $(COMMON_SRCS)
PROFILE_SRCS = $(TEST_DIR)/profile.cpp $(COMMON_SRCS)

MAIN_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(MAIN_SRCS)))
FUZZ_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(FUZZ_SRCS)))
TEST_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(TEST_SRCS)))
PROFILE_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(PROFILE_SRCS)))

DEPS = $(wildcard $(BUILD_DIR)/*.d)

MAIN_EXECUTABLE = $(BUILD_DIR)/e
FUZZ_EXECUTABLE = $(BUILD_DIR)/fuzz
TEST_EXECUTABLE = $(BUILD_DIR)/test
PROFILE_EXECUTABLE = $(BUILD_DIR)/profile

.PHONY: all fuzz test clean dev install format

all: $(BUILD_DIR) $(MAIN_EXECUTABLE)

fuzz: $(BUILD_DIR) $(FUZZ_EXECUTABLE)

test: $(BUILD_DIR) $(TEST_EXECUTABLE)
	$(TEST_EXECUTABLE)

profile: $(BUILD_DIR) $(PROFILE_EXECUTABLE)
	$(PROFILE_EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MAIN_EXECUTABLE): $(MAIN_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(FUZZ_EXECUTABLE): $(FUZZ_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(PROFILE_EXECUTABLE): $(PROFILE_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

$(BUILD_DIR)/%.o: $(UTIL_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

$(BUILD_DIR)/%.o: $(KEYBINDS_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

$(BUILD_DIR)/%.o: $(FUZZ_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

$(BUILD_DIR)/%.o: $(TEST_UTIL_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS) $(DEPFLAGS)

format:
	find . -iname "*.cpp" -o -iname "*.h" | xargs clang-format -i

dev:
	make all && $(MAIN_EXECUTABLE) test-file.h

fuzzing:
	make fuzz && $(FUZZ_EXECUTABLE)

install:
	make all && sudo mv $(MAIN_EXECUTABLE) /usr/local/bin

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
