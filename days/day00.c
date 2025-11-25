#include "solver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

const int AOC_YEAR = 2025;
const int AOC_DAY  = 1;

/*
 * Helper: return a malloc'd string from a printf-style format.
 * Just to make answers easy to construct.
 */
static char *fmt(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int len = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (len < 0) {
        va_end(args);
        return NULL;
    }
    char *buf = malloc((size_t)len + 1);
    if (!buf) {
        va_end(args);
        return NULL;
    }
    vsnprintf(buf, (size_t)len + 1, fmt, args);
    va_end(args);
    return buf;
}

char *solve_part1(const char *input) {
    return fmt("%zu", strlen(input));
}

char *solve_part2(const char *input) {
    return fmt("%zu", strlen(input) * 2);
}
