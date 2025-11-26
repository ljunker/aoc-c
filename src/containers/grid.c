#include "containers/grid.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Grid* grid_new(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Grid* g = malloc(sizeof(Grid));
    if (!g) {
        fprintf(stderr, "grid_new: out of memory\n");
        abort();
    }

    g->width = width;
    g->height = height;
    g->cells = malloc((size_t)width * (size_t)height * sizeof(char));
    if (!g->cells) {
        fprintf(stderr, "grid_new: out of memory (cells)\n");
        free(g);
        abort();
    }

    memset(g->cells, 0, (size_t)width * (size_t)height);
    return g;
}

void grid_free(Grid* g) {
    if (!g) return;
    free(g->cells);
    free(g);
}

bool grid_in_bounds(Grid* g, int x, int y) {
    if (!g) return false;
    return x >= 0 && x < g->width && y >= 0 && y < g->height;
}

char grid_get(Grid* g, int x, int y) {
    if (!g) return 0;
    if (!grid_in_bounds(g, x, y)) return 0;
    size_t idx = (size_t)y * (size_t)g->width + (size_t)x;
    return g->cells[idx];
}

void grid_set(Grid* g, int x, int y, char value) {
    if (!g) return;
    if (!grid_in_bounds(g, x, y)) return;
    size_t idx = (size_t)y * (size_t)g->width + (size_t)x;
    g->cells[idx] = value;
}

Grid* grid_from_string(const char* input) {
    if (!input) return NULL;

    int width = 0;
    const char* p = input;
    while (*p && *p != '\n') {
        width++;
        p++;
    }
    if (width == 0) {
        return NULL;
    }

    int height = 0;
    int current_len = 0;
    p = input;
    while (*p) {
        if (*p == '\n') {
            height++;
            if (current_len != width) {
                fprintf(stderr, "grid_from_string: inconsistent line width\n");
                return NULL;
            }
            current_len = 0;
        } else {
            current_len++;
        }
        p++;
    }

    if (current_len > 0) {
        height++;
        if (current_len != width) {
            fprintf(stderr, "grid_from_string: inconsistent last line width\n");
            return NULL;
        }
    }

    Grid* g = grid_new(width, height);
    if (!g) return NULL;

    int x = 0, y = 0;
    p = input;
    while (*p && y < height) {
        if (*p == '\n') {
            y++;
            x = 0;
        } else {
            if (x < width && y < height) {
                grid_set(g, x, y, *p);
            }
            x++;
        }
        p++;
    }

    return g;
}
