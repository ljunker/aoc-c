#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int AOC_YEAR = 2025;
const int AOC_DAY  = 02;

static long long pow10_arr[20];
static int pow10_init = 0;

static void init_pow10(void) {
    if (pow10_init) return;
    pow10_arr[0] = 1;
    for (int i = 1; i < 20; i++) {
        pow10_arr[i] = pow10_arr[i - 1] * 10LL;
    }
    pow10_init = 1;
}

static int num_digits(long long x) {
    if (x <= 0) return 1;
    int d = 0;
    while (x > 0) {
        x /= 10;
        d++;
    }
    return d;
}

static int cmp_ll(const void* a, const void* b) {
    long long aa = *(const long long*)a;
    long long bb = *(const long long*)b;
    if (aa < bb) return -1;
    if (aa > bb) return 1;
    return 0;
}

static void push_candidate(long long v,
                           long long **arr,
                           size_t *len,
                           size_t *cap) {
    if (*len == *cap) {
        size_t new_cap = (*cap ? *cap * 2 : 128);
        long long *new_arr = realloc(*arr, new_cap * sizeof(long long));
        if (!new_arr) {
            fprintf(stderr, "sum_invalid_in_range_k: OOM\n");
            free(*arr);
            exit(1);
        }
        *arr = new_arr;
        *cap = new_cap;
    }
    (*arr)[(*len)++] = v;
}

static long long sum_invalid_in_range_k(long long lo, long long hi, int k_min, int k_max_global) {
    init_pow10();

    if (hi < lo) {
        long long tmp = lo;
        lo = hi;
        hi = tmp;
    }
    if (hi <= 0) return 0;

    int max_digits_hi = num_digits(hi);

    long long *candidates = NULL;
    size_t cap = 0;
    size_t len = 0;

    for (int m = 1; m <= max_digits_hi / 2; m++) {
        long long start_p = pow10_arr[m - 1];
        long long end_p   = pow10_arr[m] - 1;

        int max_k_for_m = max_digits_hi / m;
        if (max_k_for_m < k_min) continue;
        if (max_k_for_m > k_max_global) max_k_for_m = k_max_global;

        for (int k = k_min; k <= max_k_for_m; k++) {
            long long mult = 0;
            for (int i = 0; i < k; i++) {
                mult = mult * pow10_arr[m] + 1;
            }

            long long min_candidate = start_p * mult;
            if (min_candidate > hi) {
                break;
            }

            for (long long p = start_p; p <= end_p; p++) {
                long long candidate = p * mult;
                if (candidate > hi) {
                    break;
                }
                if (candidate >= lo) {
                    push_candidate(candidate, &candidates, &len, &cap);
                }
            }
        }
    }

    if (len == 0) {
        free(candidates);
        return 0;
    }

    qsort(candidates, len, sizeof(long long), cmp_ll);

    long long total = 0;
    long long prev = candidates[0];
    total += prev;

    for (size_t i = 1; i < len; i++) {
        if (candidates[i] != prev) {
            total += candidates[i];
            prev = candidates[i];
        }
    }

    free(candidates);
    return total;
}

static long long solve_all_ranges(const char* input, int k_min, int k_max_global) {
    long long total = 0;
    const char* p = input;

    while (*p) {
        while (*p == ',' || *p == '\n' || *p == '\r' || *p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') break;

        long long lo, hi;
        int consumed = 0;
        if (sscanf(p, "%lld-%lld%n", &lo, &hi, &consumed) != 2) {
            break;
        }

        total += sum_invalid_in_range_k(lo, hi, k_min, k_max_global);

        p += consumed;
    }

    return total;
}

char* solve_part1(const char* input) {
    long long ans = solve_all_ranges(input, 2, 2);
    return format_string("%lld", ans);
}

char* solve_part2(const char* input) {
    long long ans = solve_all_ranges(input, 2, 100);
    return format_string("%lld", ans);
}
