# Compiler
CC = g++
CFLAGS = -Wall -lncurses

# Directories
SRC_DIR = src
KEYBINDS_DIR = $(SRC_DIR)/keybinds
UTIL_DIR = $(SRC_DIR)/util
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/edit.cpp \
          $(UTIL_DIR)/helper.cpp \
          $(UTIL_DIR)/render.cpp \
          $(UTIL_DIR)/state.cpp \
          $(KEYBINDS_DIR)/sendKeys.cpp \
          $(KEYBINDS_DIR)/sendCommandLineKeys.cpp \
          $(KEYBINDS_DIR)/sendShortcutKeys.cpp \
          $(KEYBINDS_DIR)/sendTypingKeys.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable name
EXECUTABLE = $(BUILD_DIR)/e

# Compile and link
all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/util $(BUILD_DIR)/keybinds

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(CFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
