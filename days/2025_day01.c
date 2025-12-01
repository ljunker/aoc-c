#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 1;

typedef struct {
    long end_zero_count;
    long click_zero_count;
} Result;

static long count_zeros_on_rotation(char dir, int pos, long dist) {
    if (dist <= 0)
        return 0;

    long first_k;
    if (dir == 'R' || dir == 'r') {
        first_k = (100 - pos) % 100;
        if (first_k == 0)
            first_k = 100;
    } else {
        first_k = pos % 100;
        if (first_k == 0)
            first_k = 100;
    }

    if (first_k > dist) {
        return 0;
    }
    long remaining = dist - first_k;
    return remaining / 100 + 1;
}

static Result solve_both(const char *input) {
    int pos = 50;
    long end_zero_count = 0;
    long click_zero_count = 0;

    scanner sc;
    scanner_init(&sc, input);

    char dir;
    long dist;

    while (scanner_next(&sc, "%c%ld", &dir, &dist)) {

        click_zero_count += count_zeros_on_rotation(dir, pos, dist);

        long steps = dist % 100;
        if (dir == 'L' || dir == 'l') {
            pos = (int)((pos - steps) % 100);
            if (pos < 0)
                pos += 100;
        } else if (dir == 'R' || dir == 'r') {
            pos = (int)((pos + steps) % 100);
        }

        if (pos == 0) {
            end_zero_count++;
        }
    }

    Result res = {end_zero_count, click_zero_count};
    return res;
}

char *solve_part1(const char *input) {
    Result res = solve_both(input);
    return format_string("%ld", res.end_zero_count);
}

char *solve_part2(const char *input) {
    Result res = solve_both(input);
    return format_string("%ld", res.click_zero_count);
}
