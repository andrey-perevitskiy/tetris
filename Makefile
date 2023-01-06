CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -Iinclude -g
LDFLAGS = -lcsfml-system -lcsfml-graphics -lcsfml-window -lm -g
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
BIN = $(BIN_DIR)/tetris

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DIRS = $(sort $(dir $(OBJS)))

.PHONY: all
all: $(DIRS) $(BIN_DIR) $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BIN_DIR):
	@[ -d $(BIN_DIR) ] || mkdir $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DIRS):
	@[ -d $@ ] || mkdir $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
