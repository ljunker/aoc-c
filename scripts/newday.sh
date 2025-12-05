#!/bin/sh
set -e

DAYS_DIR="days"

usage() {
    echo "Usage: $0 YEAR DAY"
    echo "  Example: $0 2025 1"
    exit 1
}

if [ $# -ne 2 ]; then
    usage
fi

YEAR="$1"
DAY="$2"

# Pad day to two digits
DAY_PADDED=$(printf "%02d" "$DAY")

FNAME="${DAYS_DIR}/${YEAR}_day${DAY_PADDED}.c"

if [ -e "$FNAME" ]; then
    echo "Error: $FNAME already exists" >&2
    exit 1
fi

mkdir -p "$DAYS_DIR"

cat >"$FNAME" <<EOF
#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int AOC_YEAR = ${YEAR};
const int AOC_DAY  = ${DAY};

char *solve_part1(const char *input) {
    (void)input;
    return format_string("0");
}

char *solve_part2(const char *input) {
    (void)input;
    return format_string("0");
}
EOF

echo "Created $FNAME"
