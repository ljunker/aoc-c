#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int AOC_YEAR = 2024;
const int AOC_DAY  = 1;

/* ---------- parsing ---------- */

static int parse_input(const char *input, int **out_left, int **out_right, size_t *out_n) {
    int *left = NULL;
    int *right = NULL;
    size_t cap = 0;
    size_t n = 0;

    const char *p = input;
    int a, b;
    int consumed;

    while (1) {
        if (sscanf(p, " %d %d%n", &a, &b, &consumed) != 2) {
            break;
        }

        if (n == cap) {
            size_t new_cap = cap ? cap * 2 : 64;
            int *new_left = realloc(left, new_cap * sizeof(int));
            int *new_right = realloc(right, new_cap * sizeof(int));
            if (!new_left || !new_right) {
                free(new_left);
                free(new_right);
                free(left);
                free(right);
                return -1;
            }
            left = new_left;
            right = new_right;
            cap = new_cap;
        }

        left[n] = a;
        right[n] = b;
        n++;

        p += consumed;
    }

    *out_left = left;
    *out_right = right;
    *out_n = n;
    return 0;
}

/* ---------- helpers ---------- */

static int cmp_int(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

static size_t lower_bound(const int *arr, size_t n, int target) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (arr[mid] < target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

static size_t upper_bound(const int *arr, size_t n, int target) {
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (arr[mid] <= target) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return lo;
}

/* ---------- part 1 ---------- */

char *solve_part1(const char *input) {
    int *left = NULL;
    int *right = NULL;
    size_t n = 0;

    if (parse_input(input, &left, &right, &n) != 0) {
        return NULL;
    }

    qsort(left, n, sizeof(int), cmp_int);
    qsort(right, n, sizeof(int), cmp_int);

    long long sum = 0;
    for (size_t i = 0; i < n; i++) {
        long long diff = (long long)left[i] - (long long)right[i];
        if (diff < 0) diff = -diff;
        sum += diff;
    }

    free(left);
    free(right);

    return format_string("%lld", sum);
}

/* ---------- part 2 ---------- */

char *solve_part2(const char *input) {
    int *left = NULL;
    int *right = NULL;
    size_t n = 0;

    if (parse_input(input, &left, &right, &n) != 0) {
        return NULL;
    }

    qsort(right, n, sizeof(int), cmp_int);

    long long score = 0;
    for (size_t i = 0; i < n; i++) {
        int value = left[i];
        size_t lb = lower_bound(right, n, value);
        size_t ub = upper_bound(right, n, value);
        size_t count = (ub > lb) ? (ub - lb) : 0;

        score += (long long)value * (long long)count;
    }

    free(left);
    free(right);

    return format_string("%lld", score);
}
