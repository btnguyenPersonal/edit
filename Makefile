# Compiler
CC = g++

# Directories
SRC_DIR = src
UTIL_DIR = $(SRC_DIR)/util
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/edit.cpp \
          $(UTIL_DIR)/helper.cpp \
          $(UTIL_DIR)/render.cpp \
          $(UTIL_DIR)/sendKeys.cpp \
          $(UTIL_DIR)/state.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable name
EXECUTABLE = $(BUILD_DIR)/e

# Compile and link
all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/util

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ -lncurses

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
