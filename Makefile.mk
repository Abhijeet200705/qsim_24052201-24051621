# Compiler and flags
CC = gcc
VPATH = src
CFLAGS = -Iinclude -Wall -g
LDFLAGS = -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Source files and object files
SOURCES = \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/simulation.c \
    $(SRC_DIR)/event.c \
    $(SRC_DIR)/queue.c

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Target executable
TARGET = $(BIN_DIR)/qSim

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# This rule tells make how to create a .o file in the obj directory
# from a .c file found in the VPATH (which is set to 'src').
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) output