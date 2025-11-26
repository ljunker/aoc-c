#include "solver.h"
#include "util.h"

#include "containers/grid.h"
#include "containers/queue.h"
#include "containers/set.h"
#include "containers/point.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/**
 * This is a test for a few containers and a BFS shortest path algorithm
 * on a grid with walls.
 */

const int AOC_YEAR = 2024;
const int AOC_DAY  = 1;

typedef struct {
    int x;
    int y;
    int dist;
} Node;

/* Turn (x,y) into a key for the visited set */
static void coord_key(char* buf, size_t bufsize, int x, int y) {
    snprintf(buf, bufsize, "%d,%d", x, y);
}

/* BFS shortest path from S to E, avoiding '#' cells.
 * Returns -1 if no path exists.
 */
static int bfs_shortest_path(Grid* g, Point start, Point goal) {
    queue* q = queue_new();
    set* visited = set_new();

    Node* start_node = malloc(sizeof(Node));
    start_node->x = start.x;
    start_node->y = start.y;
    start_node->dist = 0;
    queue_push(q, start_node);

    char keybuf[64];
    coord_key(keybuf, sizeof(keybuf), start.x, start.y);
    set_add(visited, keybuf);

    int answer = -1;

    while (!queue_empty(q)) {
        Node* cur = (Node*)queue_pop(q);
        int x = cur->x;
        int y = cur->y;
        int dist = cur->dist;

        if (x == goal.x && y == goal.y) {
            answer = dist;
            free(cur);
            break;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + DIRS4[i].x;
            int ny = y + DIRS4[i].y;

            if (!grid_in_bounds(g, nx, ny)) continue;
            char cell = grid_get(g, nx, ny);
            if (cell == '#') continue;  // wall

            coord_key(keybuf, sizeof(keybuf), nx, ny);
            if (set_contains(visited, keybuf)) continue;

            fprintf(stderr, "Visiting (%d,%d) at dist %d\n", nx, ny, dist + 1);

            set_add(visited, keybuf);

            Node* next = malloc(sizeof(Node));
            next->x = nx;
            next->y = ny;
            next->dist = dist + 1;
            queue_push(q, next);
        }

        free(cur);
    }

    while (!queue_empty(q)) {
        Node* n = (Node*)queue_pop(q);
        free(n);
    }

    queue_free(q);
    set_free(visited);

    return answer;
}

/* Find S and E in the grid. Returns 0 on success, -1 if not found. */
static int find_start_and_goal(Grid* g, Point* out_start, Point* out_goal) {
    int found_s = 0, found_e = 0;
    for (int y = 0; y < g->height; y++) {
        for (int x = 0; x < g->width; x++) {
            char c = grid_get(g, x, y);
            if (c == 'S') {
                out_start->x = x;
                out_start->y = y;
                found_s = 1;
            } else if (c == 'E') {
                out_goal->x = x;
                out_goal->y = y;
                found_e = 1;
            }
        }
    }
    return (found_s && found_e) ? 0 : -1;
}

char* solve_part1(const char* input) {
    Grid* g = grid_from_string(input);
    if (!g) {
        return format_string("0");
    }

    Point start = {0, 0};
    Point goal = {0, 0};

    if (find_start_and_goal(g, &start, &goal) != 0) {
        grid_free(g);
        return format_string("0");
    }

    int dist = bfs_shortest_path(g, start, goal);
    grid_free(g);

    if (dist < 0) {
        return format_string("0");
    }

    return format_string("%d", dist);
}

char* solve_part2(const char* input) {
    return solve_part1(input);
}
