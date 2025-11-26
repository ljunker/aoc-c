// grid.h
#ifndef GRID_H
#define GRID_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    int width;
    int height;
    char *cells;
} Grid;

Grid* grid_new(int width, int height);
void grid_free(Grid* g);

bool grid_in_bounds(Grid* g, int x, int y);

char grid_get(Grid* g, int x, int y);
void grid_set(Grid* g, int x, int y, char value);

Grid* grid_from_string(const char* input);

#endif
