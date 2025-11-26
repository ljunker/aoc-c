// point.h
#ifndef POINT_H
#define POINT_H

typedef struct {
    int x;
    int y;
} Point;

static const Point DIRS4[4] = {
    { 1, 0 },  // right
    {-1, 0 },  // left
    { 0, 1 },  // down
    { 0,-1 }   // up
};

static const Point DIRS8[8] = {
    { 1, 0}, {-1, 0}, {0, 1}, {0,-1},
    { 1, 1}, { 1,-1}, {-1, 1}, {-1,-1}
};

#endif
