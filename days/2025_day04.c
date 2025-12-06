#include "solver.h"
#include "util.h"

#include "containers/grid.h"

#include <stdlib.h>
#include <stdint.h>

const int AOC_YEAR = 2025;
const int AOC_DAY  = 4;

static void analyze_grid(const char* input,
                         int* out_accessible_initial,
                         int* out_removed_total) {
    Grid* g = grid_from_string(input);

    const int w = g->width;
    const int h = g->height;
    const int n = w * h;

    char* cells = g->cells;

    uint8_t* degree = malloc((size_t)n * sizeof(uint8_t));
    int*     queue  = malloc((size_t)n * sizeof(int));

    const int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    const int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};

    int accessible_initial = 0;
    int head = 0, tail = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const int idx = y * w + x;
            if (cells[idx] != '@') {
                degree[idx] = 0;
                continue;
            }

            uint8_t count = 0;
            for (int dir = 0; dir < 8; dir++) {
                const int nx = x + dx[dir];
                const int ny = y + dy[dir];
                if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
                const int nidx = ny * w + nx;
                if (cells[nidx] == '@') {
                    count++;
                }
            }
            degree[idx] = count;

            if (count < 4) {
                accessible_initial++;
                queue[tail++] = idx;
            }
        }
    }

    int removed_total = 0;

    while (head < tail) {
        const int idx = queue[head++];
        if (cells[idx] != '@') continue;

        cells[idx] = '.';
        removed_total++;

        const int x = idx % w;
        const int y = idx / w;

        for (int dir = 0; dir < 8; dir++) {
            const int nx = x + dx[dir];
            const int ny = y + dy[dir];
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;

            const int nidx = ny * w + nx;
            if (cells[nidx] != '@') continue;

            uint8_t d = degree[nidx];
            if (d > 0) {
                d--;
                degree[nidx] = d;
                if (d == 3) {
                    queue[tail++] = nidx;
                }
            }
        }
    }

    free(queue);
    free(degree);
    grid_free(g);

    *out_accessible_initial = accessible_initial;
    *out_removed_total = removed_total;
}

char* solve_part1(const char* input) {
    int accessible_initial = 0;
    int removed_total = 0;
    analyze_grid(input, &accessible_initial, &removed_total);
    (void)removed_total;
    return format_string("%lld", accessible_initial);
}

char* solve_part2(const char* input) {
    int accessible_initial = 0;
    int removed_total = 0;
    analyze_grid(input, &accessible_initial, &removed_total);
    (void)accessible_initial;
    return format_string("%lld", removed_total);
}
