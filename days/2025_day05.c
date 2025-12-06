#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/errno.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 5;

typedef struct {
    long long lo;
    long long hi;
} Range;

static int cmp_range(const void *a, const void *b) {
    const Range *ra = a;
    const Range *rb = b;
    if (ra->lo < rb->lo) return -1;
    if (ra->lo > rb->lo) return 1;
    if (ra->hi < rb->hi) return -1;
    if (ra->hi > rb->hi) return 1;
    return 0;
}

static void parse_database(
    const char *input,
    Range **out_ranges, size_t *out_ranges_len,
    long long **out_ids, size_t *out_ids_len
) {
    const char *p = input;
    bool in_ranges = true;

    Range *ranges = NULL;
    size_t ranges_len = 0, ranges_cap = 0;

    long long *ids = NULL;
    size_t ids_len = 0, ids_cap = 0;

    while (*p) {
        int count_newlines = 0;
        while (*p == '\n' || *p == '\r') {
            p++;
            count_newlines++;
        }
        if (count_newlines == 2) {
            in_ranges = false;
            continue;
        }

        const char *start = p;
        while (*p && *p != '\n' && *p != '\r') {
            p++;
        }
        size_t len = (size_t) (p - start);
        if (len == 0) {
            continue;
        }

        char buf[128];
        if (len >= sizeof(buf)) len = sizeof(buf) - 1;
        memcpy(buf, start, len);
        buf[len] = '\0';

        if (in_ranges) {
            char *endptr;

            errno = 0;
            const long long lo = strtoll(buf, &endptr, 10);
            if (endptr == buf) {
                continue;
            }
            while (*endptr == ' ' || *endptr == '\t') endptr++;

            if (*endptr != '-') {
                continue;
            }
            endptr++;
            while (*endptr == ' ' || *endptr == '\t') endptr++;

            errno = 0;
            const long long hi = strtoll(endptr, &endptr, 10);
            if (endptr == NULL || *(endptr - 1) == '\0') {
                (void) 0;
            }
            if (errno != 0) {
                continue;
            }

            if (ranges_len == ranges_cap) {
                const size_t new_cap = ranges_cap ? ranges_cap * 2 : 16;
                Range *tmp = realloc(ranges, new_cap * sizeof(Range));
                if (!tmp) {
                    fprintf(stderr, "OOM in parse ranges\n");
                    free(ranges);
                    free(ids);
                    abort();
                }
                ranges = tmp;
                ranges_cap = new_cap;
            }
            Range r;
            if (lo <= hi) {
                r.lo = lo;
                r.hi = hi;
            } else {
                r.lo = hi;
                r.hi = lo;
            }
            if (!ranges) {
                fprintf(stderr, "OOM in parse ranges\n");
                free(ranges);
                free(ids);
                abort();
            }
            ranges[ranges_len++] = r;
        } else {
            char *endptr;

            errno = 0;
            const long long id = strtoll(buf, &endptr, 10);
            if (endptr == buf || errno != 0) {
                continue;
            }

            if (ids_len == ids_cap) {
                const size_t new_cap = ids_cap ? ids_cap * 2 : 16;
                long long *tmp = realloc(ids, new_cap * sizeof(long long));
                if (!tmp) {
                    fprintf(stderr, "OOM in parse ids\n");
                    free(ranges);
                    free(ids);
                    abort();
                }
                ids = tmp;
                ids_cap = new_cap;
            }
            if (!ids) {
                fprintf(stderr, "OOM in parse ranges\n");
                free(ranges);
                free(ids);
                abort();
            }
            ids[ids_len++] = id;
        }
    }

    *out_ranges = ranges;
    *out_ranges_len = ranges_len;
    *out_ids = ids;
    *out_ids_len = ids_len;
}

static size_t merge_ranges(Range *ranges, const size_t len) {
    if (len == 0) return 0;

    qsort(ranges, len, sizeof(Range), cmp_range);

    size_t out = 0;
    for (size_t i = 0; i < len; i++) {
        if (out == 0) {
            ranges[out++] = ranges[i];
        } else {
            Range *last = &ranges[out - 1];
            const Range curr = ranges[i];
            if (curr.lo <= last->hi) {
                if (curr.hi > last->hi) {
                    last->hi = curr.hi;
                }
            } else {
                ranges[out++] = curr;
            }
        }
    }
    return out;
}

static bool id_is_fresh(const long long id, const Range *ranges, const size_t len) {
    if (len == 0) return false;

    size_t lo = 0, hi = len - 1;
    while (lo <= hi) {
        const size_t mid = lo + (hi - lo) / 2;
        const Range r = ranges[mid];
        if (id < r.lo) {
            if (mid == 0) break;
            hi = mid - 1;
        } else if (id > r.hi) {
            lo = mid + 1;
        } else {
            return true;
        }
    }
    return false;
}

static long long count_fresh(const char *input) {
    Range *ranges = NULL;
    size_t ranges_len = 0;
    long long *ids = NULL;
    size_t ids_len = 0;

    parse_database(input, &ranges, &ranges_len, &ids, &ids_len);

    const size_t merged_len = merge_ranges(ranges, ranges_len);

    long long fresh_count = 0;
    for (size_t i = 0; i < ids_len; i++) {
        if (id_is_fresh(ids[i], ranges, merged_len)) {
            fresh_count++;
        }
    }

    free(ranges);
    free(ids);
    return fresh_count;
}

static long long count_fresh_range_space(const char *input) {
    Range *ranges = NULL;
    size_t ranges_len = 0;
    long long *ids = NULL;
    size_t ids_len = 0;

    parse_database(input, &ranges, &ranges_len, &ids, &ids_len);

    free(ids);

    const size_t merged_len = merge_ranges(ranges, ranges_len);

    long long total_ids = 0;
    for (size_t i = 0; i < merged_len; i++) {
        const long long lo = ranges[i].lo;
        const long long hi = ranges[i].hi;
        total_ids += hi - lo + 1;
    }

    free(ranges);
    return total_ids;
}

char *solve_part1(const char *input) {
    const long long ans = count_fresh(input);
    return format_string("%lld", ans);
}

char *solve_part2(const char *input) {
    const long long ans = count_fresh_range_space(input);
    return format_string("%lld", ans);
}
