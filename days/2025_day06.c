#include "solver.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 6;

typedef struct {
    size_t rows;
    size_t cols;
    char *grid;
    bool *sep;
} Worksheet;

static bool parse_worksheet(const char *input, Worksheet *ws) {
    if (!input || !*input) {
        ws->rows = ws->cols = 0;
        ws->grid = NULL;
        ws->sep = NULL;
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
        ws->rows = ws->cols = 0;
        ws->grid = NULL;
        ws->sep = NULL;
        return false;
    }

    char *grid = malloc(rows * cols);
    if (!grid) {
        return false;
    }
    memset(grid, ' ', rows * cols);

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

    bool *sep = malloc(cols * sizeof(bool));
    if (!sep) {
        free(grid);
        return false;
    }

    for (size_t col = 0; col < cols; ++col) {
        bool all_space = true;
        for (size_t row = 0; row < rows; ++row) {
            if (grid[row * cols + col] != ' ') {
                all_space = false;
                break;
            }
        }
        sep[col] = all_space;
    }

    ws->rows = rows;
    ws->cols = cols;
    ws->grid = grid;
    ws->sep = sep;
    return true;
}

static void free_worksheet(Worksheet *ws) {
    free(ws->grid);
    free(ws->sep);
    ws->grid = NULL;
    ws->sep = NULL;
    ws->rows = ws->cols = 0;
}

static long long compute_grand_total_rowwise(const char *input) {
    Worksheet ws;
    if (!parse_worksheet(input, &ws)) {
        return 0;
    }

    long long grand_total = 0;
    if (ws.rows == 0 || ws.cols == 0) {
        free_worksheet(&ws);
        return 0;
    }

    const size_t last_row = ws.rows - 1;
    size_t col = 0;

    while (col < ws.cols) {
        if (ws.sep[col]) {
            col++;
            continue;
        }

        const size_t start = col;
        while (col < ws.cols && !ws.sep[col]) {
            col++;
        }
        const size_t end = col;

        char op = 0;
        for (size_t cc = start; cc < end; ++cc) {
            const char ch = ws.grid[last_row * ws.cols + cc];
            if (ch == '+' || ch == '*') {
                op = ch;
                break;
            }
        }
        if (op == 0) {
            continue;
        }

        long long result = op == '+' ? 0 : 1;
        bool have_any_number = false;

        for (size_t row = 0; row + 1 < ws.rows; ++row) {
            char buf[64];
            size_t blen = 0;

            for (size_t cc = start; cc < end; ++cc) {
                const char ch = ws.grid[row * ws.cols + cc];
                if (isdigit((unsigned char) ch)) {
                    if (blen + 1 < sizeof(buf)) {
                        buf[blen++] = ch;
                    }
                }
            }

            if (blen == 0) continue;

            buf[blen] = '\0';
            const long long val = strtoll(buf, NULL, 10);
            have_any_number = true;

            if (op == '+') {
                result += val;
            } else {
                result *= val;
            }
        }

        if (have_any_number) {
            grand_total += result;
        }
    }

    free_worksheet(&ws);
    return grand_total;
}

static long long compute_grand_total_colwise(const char *input) {
    Worksheet ws;
    if (!parse_worksheet(input, &ws)) {
        return 0;
    }

    long long grand_total = 0;
    if (ws.rows == 0 || ws.cols == 0) {
        free_worksheet(&ws);
        return 0;
    }

    const size_t last_row = ws.rows - 1;
    size_t col = 0;

    while (col < ws.cols) {
        if (ws.sep[col]) {
            col++;
            continue;
        }

        const size_t start = col;
        while (col < ws.cols && !ws.sep[col]) {
            col++;
        }
        const size_t end = col;

        char op = 0;
        for (size_t cc = start; cc < end; ++cc) {
            const char ch = ws.grid[last_row * ws.cols + cc];
            if (ch == '+' || ch == '*') {
                op = ch;
                break;
            }
        }
        if (op == 0) {
            continue;
        }

        long long result = op == '+' ? 0 : 1;
        bool have_any_number = false;

        for (size_t cc = end; cc-- > start;) {
            char buf[64];
            size_t blen = 0;


            for (size_t row = 0; row < last_row; ++row) {
                const char ch = ws.grid[row * ws.cols + cc];
                if (isdigit((unsigned char) ch)) {
                    if (blen + 1 < sizeof(buf)) {
                        buf[blen++] = ch;
                    }
                }
            }

            if (blen == 0) continue;

            buf[blen] = '\0';
            const long long val = strtoll(buf, NULL, 10);
            have_any_number = true;

            if (op == '+') {
                result += val;
            } else {
                result *= val;
            }
        }

        if (have_any_number) {
            grand_total += result;
        }
    }

    free_worksheet(&ws);
    return grand_total;
}

char *solve_part1(const char *input) {
    const long long total = compute_grand_total_rowwise(input);
    return format_string("%lld", total);
}

char *solve_part2(const char *input) {
    const long long total = compute_grand_total_colwise(input);
    return format_string("%lld", total);
}
