#include "solver.h"
#include "util.h"

#include "containers/numset.h"

#include <stdlib.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 02;

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

static long long sum_invalid_in_range_k(long long lo, long long hi, const int k_max_global) {
    init_pow10();

    if (hi < lo) {
        const long long tmp = lo;
        lo = hi;
        hi = tmp;
    }
    if (hi <= 0) return 0;

    const int max_digits_hi = num_digits(hi);

    numset *S = numset_new();

    for (int m = 1; m <= max_digits_hi / 2; m++) {
        const int k_min = 2;
        const long long start_p = pow10_arr[m - 1];
        const long long end_p = pow10_arr[m] - 1;

        int max_k_for_m = max_digits_hi / m;
        if (max_k_for_m < k_min) continue;
        if (max_k_for_m > k_max_global) max_k_for_m = k_max_global;

        for (int k = k_min; k <= max_k_for_m; k++) {
            long long mult = 0;
            for (int i = 0; i < k; i++) {
                mult = mult * pow10_arr[m] + 1;
            }

            const long long min_candidate = start_p * mult;
            if (min_candidate > hi) {
                break;
            }

            for (long long p = start_p; p <= end_p; p++) {
                const long long candidate = p * mult;
                if (candidate > hi) {
                    break;
                }
                if (candidate >= lo) {
                    numset_add(S, candidate);
                }
            }
        }
    }

    long long total = 0;
    numset_iter it = numset_iterator(S);
    long long v;
    while (numset_next(&it, &v)) {
        total += v;
    }

    numset_free(S);
    return total;
}

static long long solve_all_ranges(const char *input, const int k_max_global) {
    long long total = 0;
    const char *p = input;

    while (*p) {
        // Skip separators
        while (*p == ',' || *p == '\n' || *p == '\r' ||
               *p == ' ' || *p == '\t') {
            p++;
               }
        if (!*p) break;

        // ---- parse lo ----
        char *endptr;
        const long long lo = strtoll(p, &endptr, 10);
        if (endptr == p) {
            // no number found → stop
            break;
        }
        p = endptr;

        // expect '-'
        if (*p != '-') {
            break; // malformed range
        }
        p++;

        // ---- parse hi ----
        const long long hi = strtoll(p, &endptr, 10);
        if (endptr == p) {
            break; // malformed range
        }
        p = endptr;

        total += sum_invalid_in_range_k(lo, hi, k_max_global);
    }

    return total;
}


char *solve_part1(const char *input) {
    const long long ans = solve_all_ranges(input, 2);
    return format_string("%lld", ans);
}

char *solve_part2(const char *input) {
    const long long ans = solve_all_ranges(input, 100);
    return format_string("%lld", ans);
}
