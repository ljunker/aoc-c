#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 7;

typedef struct {
    size_t rows;
    size_t cols;
    char *grid;
} Grid;

static bool parse_grid(const char *input, Grid *g) {
    if (!input || !*input) {
        g->rows = g->cols = 0;
        g->grid = NULL;
        return false;
    }

    size_t rows = 0;
    size_t cols = 0;
    size_t cur_len = 0;

    for (const char *p = input; ; ++p) {
        const char ch = *p;
        if (ch == '\n' || ch == '\0') {
            if (cur_len > cols) {
                cols = cur_len;
            }
            rows++;
            cur_len = 0;
            if (ch == '\0') {
                break;
            }
        } else if (ch == '\r') {
        } else {
            cur_len++;
        }
    }

    if (rows == 0 || cols == 0) {
        g->rows = g->cols = 0;
        g->grid = NULL;
        return false;
    }

    char *grid = malloc(rows * cols);
    if (!grid) {
        g->rows = g->cols = 0;
        g->grid = NULL;
        return false;
    }

    memset(grid, '.', rows * cols);

    size_t r = 0;
    size_t c = 0;
    for (const char *p = input; ; ++p) {
        const char ch = *p;
        if (ch == '\n' || ch == '\0') {
            r++;
            c = 0;
            if (ch == '\0' || r >= rows) {
                break;
            }
        } else if (ch == '\r') {
        } else {
            if (c < cols) {
                grid[r * cols + c] = ch;
                c++;
            }
        }
    }

    g->rows = rows;
    g->cols = cols;
    g->grid = grid;
    return true;
}

static void free_grid(Grid *g) {
    free(g->grid);
    g->grid = NULL;
    g->rows = g->cols = 0;
}

static long long count_splits(const char *input) {
    Grid g;
    if (!parse_grid(input, &g)) {
        return 0;
    }

    if (g.rows == 0 || g.cols == 0 || !g.grid) {
        free_grid(&g);
        return 0;
    }

    size_t start_row = 0;
    size_t start_col = 0;
    bool found_start = false;

    for (size_t r = 0; r < g.rows && !found_start; ++r) {
        for (size_t c = 0; c < g.cols; ++c) {
            if (g.grid[r * g.cols + c] == 'S') {
                start_row = r;
                start_col = c;
                found_start = true;
                break;
            }
        }
    }

    if (!found_start) {
        free_grid(&g);
        return 0;
    }

    bool *curr = calloc(g.cols, sizeof(bool));
    bool *next = calloc(g.cols, sizeof(bool));
    if (!curr || !next) {
        free(curr);
        free(next);
        free_grid(&g);
        return 0;
    }

    long long splits = 0;

    curr[start_col] = true;

    for (size_t r = start_row + 1; r < g.rows; ++r) {
        memset(next, 0, g.cols * sizeof(bool));

        for (size_t c = 0; c < g.cols; ++c) {
            if (!curr[c]) continue;

            const char ch = g.grid[r * g.cols + c];

            if (ch == '^') {
                splits++;
                if (c > 0) {
                    next[c - 1] = true;
                }
                if (c + 1 < g.cols) {
                    next[c + 1] = true;
                }
            } else {
                next[c] = true;
            }
        }

        bool *tmp = curr;
        curr = next;
        next = tmp;
    }

    free(curr);
    free(next);
    free_grid(&g);
    return splits;
}

static long long count_timelines(const char *input) {
    Grid g;
    if (!parse_grid(input, &g)) {
        return 0;
    }

    if (g.rows == 0 || g.cols == 0 || !g.grid) {
        free_grid(&g);
        return 0;
    }

    size_t start_row = 0;
    size_t start_col = 0;
    bool found_start = false;

    for (size_t r = 0; r < g.rows && !found_start; ++r) {
        for (size_t c = 0; c < g.cols; ++c) {
            if (g.grid[r * g.cols + c] == 'S') {
                start_row = r;
                start_col = c;
                found_start = true;
                break;
            }
        }
    }

    if (!found_start) {
        free_grid(&g);
        return 0;
    }

    long long *curr = calloc(g.cols, sizeof(long long));
    long long *next = calloc(g.cols, sizeof(long long));
    if (!curr || !next) {
        free(curr);
        free(next);
        free_grid(&g);
        return 0;
    }

    curr[start_col] = 1;

    for (size_t r = start_row + 1; r < g.rows; ++r) {
        memset(next, 0, g.cols * sizeof(long long));

        for (size_t c = 0; c < g.cols; ++c) {
            const long long ways = curr[c];
            if (ways == 0) continue;

            const char ch = g.grid[r * g.cols + c];

            if (ch == '^') {
                if (c > 0) {
                    next[c - 1] += ways;
                }
                if (c + 1 < g.cols) {
                    next[c + 1] += ways;
                }
            } else {
                next[c] += ways;
            }
        }

        long long *tmp = curr;
        curr = next;
        next = tmp;
    }

    long long total = 0;
    for (size_t c = 0; c < g.cols; ++c) {
        total += curr[c];
    }

    free(curr);
    free(next);
    free_grid(&g);
    return total;
}

char *solve_part1(const char *input) {
    const long long total_splits = count_splits(input);
    return format_string("%lld", total_splits);
}

char *solve_part2(const char *input) {
    const long long timelines = count_timelines(input);
    return format_string("%lld", timelines);
}
