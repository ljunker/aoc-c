#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

const int AOC_YEAR = 2025;
const int AOC_DAY  = 3;

// Part 1

static int max_2digits_for_line(const char* line, const size_t len) {
    int *digits = malloc(len * sizeof(int));

    for (size_t i = 0; i < len; i++) {
        const char c = line[i];
        if (c >= '0' && c <= '9') {
            digits[i] = c - '0';
        }
    }

    int *max_right = malloc(len * sizeof(int));

    int cur = -1;
    for (ssize_t i = (ssize_t)len - 1; i >= 0; i--) {
        max_right[i] = cur;
        if (digits[i] > cur) cur = digits[i];
    }

    int best = 0;
    for (size_t i = 0; i < len; i++) {
        if (max_right[i] >= 0) {
            const int val = digits[i] * 10 + max_right[i];
            if (val > best) best = val;
        }
    }

    free(digits);
    free(max_right);
    return best;
}

// Part 2

static long long max_12digits_for_line(const char* line, const size_t len) {
    const int K = 12;

    int *digits = malloc(len * sizeof(int));

    for (size_t i = 0; i < len; i++) {
        const char c = line[i];
        if (c >= '0' && c <= '9') {
            digits[i] = c - '0';
        }
    }

    long long value = 0;
    ssize_t prev_index = -1;

    for (int pos = 0; pos < K; pos++) {
        const int remaining = K - pos;
        const ssize_t left  = prev_index + 1;
        const ssize_t right = (ssize_t)len - remaining;

        int best_digit = -1;
        ssize_t best_idx = left;

        for (ssize_t j = left; j <= right; j++) {
            const int d = digits[j];
            if (d > best_digit) {
                best_digit = d;
                best_idx = j;
                if (best_digit == 9) {
                    break;
                }
            }
        }

        prev_index = best_idx;
        value = value * 10 + best_digit;
    }

    free(digits);
    return value;
}

static long long solve_total_part1(const char* input) {
    long long total = 0;
    const char* p = input;

    while (*p) {
        const char* start = p;
        while (*p && *p != '\n' && *p != '\r') {
            p++;
        }
        const size_t len = (size_t)(p - start);
        if (len > 0) {
            total += max_2digits_for_line(start, len);
        }
        while (*p == '\n' || *p == '\r') {
            p++;
        }
    }

    return total;
}

static long long solve_total_part2(const char* input) {
    long long total = 0;
    const char* p = input;

    while (*p) {
        const char* start = p;
        while (*p && *p != '\n' && *p != '\r') {
            p++;
        }
        const size_t len = (size_t)(p - start);
        if (len > 0) {
            total += max_12digits_for_line(start, len);
        }
        while (*p == '\n' || *p == '\r') {
            p++;
        }
    }

    return total;
}

char* solve_part1(const char* input) {
    const long long ans = solve_total_part1(input);
    return format_string("%lld", ans);
}

char* solve_part2(const char* input) {
    const long long ans = solve_total_part2(input);
    return format_string("%lld", ans);
}
