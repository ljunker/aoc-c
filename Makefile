CC      := clang
CFLAGS  := -Wall -Wextra -std=c11 -O2 -Isrc
LDFLAGS := -lcurl

SRC_DIR := src
DAY_DIR := days
OBJ_DIR := build
BIN_DIR := bin

# -------------------------------------------------------
# Automatically find all <YEAR>_dayNN.c files
# -------------------------------------------------------
DAY_SOURCES := $(wildcard $(DAY_DIR)/*_day*.c)
DAYS        := $(basename $(notdir $(DAY_SOURCES)))
# Example: days/2025_day01.c -> 2025_day01

CORE_SRCS := $(SRC_DIR)/aoc_client.c \
             $(SRC_DIR)/util.c \
             $(SRC_DIR)/runner.c

CORE_OBJS := $(CORE_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DAY_OBJS  := $(DAYS:%=$(OBJ_DIR)/%.o)
DAY_BINS  := $(DAYS:%=$(BIN_DIR)/%)

.PHONY: all clean $(DAYS) \
        run-%-part1 run-%-part2 \
        submit-%-part1 submit-%-part2 \
        newday
		today

all: $(DAY_BINS)

$(DAYS): %: $(BIN_DIR)/%

$(BIN_DIR)/%: $(CORE_OBJS) $(OBJ_DIR)/%.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(DAY_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# ------------------ run/submit convenience -------------------------

run-%-part1: $(BIN_DIR)/%
	./$< --part 1

run-%-part2: $(BIN_DIR)/%
	./$< --part 2

submit-%-part1: $(BIN_DIR)/%
	./$< --part 1 --submit

submit-%-part2: $(BIN_DIR)/%
	./$< --part 2 --submit

# ------------------ generator: make newday YEAR=2025 DAY=1 ---------

newday:
	@if [ -z "$(YEAR)" ] || [ -z "$(DAY)" ]; then \
		echo "Usage: make newday YEAR=2025 DAY=1"; \
		exit 1; \
	fi; \
	day_padded=`printf "%02d" $(DAY)`; \
	fname="$(DAY_DIR)/$(YEAR)_day$$day_padded.c"; \
	if [ -e "$$fname" ]; then \
		echo "Error: $$fname already exists"; \
		exit 1; \
	fi; \
	mkdir -p $(DAY_DIR); \
	echo "#include \"solver.h\"" >  "$$fname"; \
	echo "#include \"util.h\""   >> "$$fname"; \
	echo ""                       >> "$$fname"; \
	echo "#include <stdlib.h>"   >> "$$fname"; \
	echo "#include <stdio.h>"    >> "$$fname"; \
	echo "#include <string.h>"   >> "$$fname"; \
	echo ""                       >> "$$fname"; \
	echo "const int AOC_YEAR = $(YEAR);" >> "$$fname"; \
	echo "const int AOC_DAY  = $(DAY);"  >> "$$fname"; \
	echo ""                       >> "$$fname"; \
	echo "char *solve_part1(const char *input) {" >> "$$fname"; \
	echo "    (void)input;"       >> "$$fname"; \
	echo "    return format_string(\"0\");" >> "$$fname"; \
	echo "}"                      >> "$$fname"; \
	echo ""                       >> "$$fname"; \
	echo "char *solve_part2(const char *input) {" >> "$$fname"; \
	echo "    (void)input;"       >> "$$fname"; \
	echo "    return format_string(\"0\");" >> "$$fname"; \
	echo "}"                      >> "$$fname"; \
	echo "Created $$fname"

today:
	@year=$$(date +%Y); \
	month=$$(date +%m); \
	day=$$(date +%d); \
	if [ "$$month" != "12" ]; then \
		echo "Warning: It's month $$month, AoC is usually in December."; \
	fi; \
	echo "Creating $$year day $$day..."; \
	$(MAKE) newday YEAR=$$year DAY=$$day
