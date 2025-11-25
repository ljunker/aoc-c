CC      := clang
CFLAGS  := -Wall -Wextra -Werror -std=c11 -O2 -Isrc
LDFLAGS := -lcurl

SRC_DIR := src
DAY_DIR := days
OBJ_DIR := build
BIN_DIR := bin

DAY_SOURCES := $(wildcard $(DAY_DIR)/*.c)
DAYS := $(basename $(notdir $(DAY_SOURCES)))

CORE_SRCS := $(SRC_DIR)/aoc_client.c \
             $(SRC_DIR)/util.c \
             $(SRC_DIR)/runner.c

CORE_OBJS := $(CORE_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DAY_OBJS  := $(DAYS:%=$(OBJ_DIR)/%.o)
DAY_BINS  := $(DAYS:%=$(BIN_DIR)/%)

.PHONY: all clean $(DAYS) \
        run-%-part1 run-%-part2 \
        submit-%-part1 submit-%-part2

all: $(DAY_BINS)

$(DAYS): %: $(BIN_DIR)/%

$(BIN_DIR)/%: $(CORE_OBJS) $(OBJ_DIR)/%.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Core objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Day objects (from days/)
$(OBJ_DIR)/%.o: $(DAY_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)


run-%-part1: $(BIN_DIR)/%
	./$< --part 1

run-%-part2: $(BIN_DIR)/%
	./$< --part 2

submit-%-part1: $(BIN_DIR)/%
	./$< --part 1 --submit

submit-%-part2: $(BIN_DIR)/%
	./$< --part 2 --submit
