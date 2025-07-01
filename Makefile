CC = g++

NPROCS := $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu)
MAKEFLAGS += --jobs=$(NPROCS)

CFLAGS   = -Wall -O2
LDFLAGS  = -lncurses -pthread
DEPFLAGS = -MMD -MP

SRC_DIR     = src
UTIL_DIR    = $(SRC_DIR)/util
KEYBINDS_DIR= $(SRC_DIR)/keybinds
FUZZ_DIR    = $(SRC_DIR)/fuzzer
TEST_DIR    = test
BUILD_DIR   = build

COMMON_SOURCES := \
  $(UTIL_DIR)/helper.cpp \
  $(UTIL_DIR)/render.cpp \
  $(UTIL_DIR)/fileExplorerNode.cpp \
  $(UTIL_DIR)/state.cpp \
  $(UTIL_DIR)/clipboard.cpp \
  $(UTIL_DIR)/history.cpp \
  $(UTIL_DIR)/indent.cpp \
  $(UTIL_DIR)/comment.cpp \
  $(UTIL_DIR)/insertLoggingCode.cpp \
  $(UTIL_DIR)/query.cpp \
  $(UTIL_DIR)/name.cpp \
  $(UTIL_DIR)/cleanup.cpp \
  $(UTIL_DIR)/keys.cpp \
  $(KEYBINDS_DIR)/sendKeys.cpp \
  $(KEYBINDS_DIR)/sendVisualKeys.cpp \
  $(KEYBINDS_DIR)/sendBlameKeys.cpp \
  $(KEYBINDS_DIR)/sendFindFileKeys.cpp \
  $(KEYBINDS_DIR)/sendFileExplorerKeys.cpp \
  $(KEYBINDS_DIR)/sendSearchKeys.cpp \
  $(KEYBINDS_DIR)/sendGrepKeys.cpp \
  $(KEYBINDS_DIR)/sendCommandLineKeys.cpp \
  $(KEYBINDS_DIR)/sendShortcutKeys.cpp \
  $(KEYBINDS_DIR)/sendMultiCursorKeys.cpp \
  $(KEYBINDS_DIR)/sendTypingKeys.cpp

MAIN_SOURCES = $(SRC_DIR)/edit.cpp $(COMMON_SOURCES)
FUZZ_SOURCES = $(FUZZ_DIR)/fuzzer.cpp $(COMMON_SOURCES)
TEST_SOURCES = $(TEST_DIR)/testAll.cpp $(COMMON_SOURCES)

MAIN_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(MAIN_SOURCES)))
FUZZ_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(FUZZ_SOURCES)))
TEST_OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(TEST_SOURCES)))

DEPS = $(wildcard $(BUILD_DIR)/*.d)

MAIN_EXECUTABLE = $(BUILD_DIR)/e
FUZZ_EXECUTABLE = $(BUILD_DIR)/fuzz
TEST_EXECUTABLE = $(BUILD_DIR)/test

.PHONY: all fuzz test clean dev install format

all: $(BUILD_DIR) $(MAIN_EXECUTABLE)

fuzz: $(BUILD_DIR) $(FUZZ_EXECUTABLE)

test: $(BUILD_DIR) $(TEST_EXECUTABLE)
	$(TEST_EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MAIN_EXECUTABLE): $(MAIN_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(FUZZ_EXECUTABLE): $(FUZZ_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) $(CFLAGS)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
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
