#include "solver.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

const int AOC_YEAR = 2025;
const int AOC_DAY = 8;

typedef struct {
    long long x, y, z;
} Point;

typedef struct {
    unsigned long long dist;
    int a;
    int b;
} Edge;

static bool parse_xyz(const char *line, long long *x, long long *y, long long *z) {
    char *end;

    *x = strtoll(line, &end, 10);
    if (end == line) return false;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != ',') return false;
    end++;

    const char *p = end;
    *y = strtoll(p, &end, 10);
    if (end == p) return false;
    while (*end == ' ' || *end == '\t') end++;
    if (*end != ',') return false;
    end++;

    p = end;
    *z = strtoll(p, &end, 10);
    if (end == p) return false;

    return true;
}

static size_t parse_points(const char *input, Point **out_points) {
    if (!input) {
        *out_points = NULL;
        return 0;
    }

    size_t cap = 64;
    size_t len = 0;
    Point *pts = malloc(cap * sizeof(Point));
    if (!pts) {
        *out_points = NULL;
        return 0;
    }

    const char *p = input;
    while (*p) {
        while (*p == '\n' || *p == '\r') p++;
        if (!*p) break;

        const char *line_start = p;
        while (*p && *p != '\n' && *p != '\r') p++;
        const char *line_end = p;

        char buf[128];
        size_t line_len = (size_t) (line_end - line_start);
        if (line_len >= sizeof(buf)) line_len = sizeof(buf) - 1;
        memcpy(buf, line_start, line_len);
        buf[line_len] = '\0';

        long long x, y, z;
        if (parse_xyz(buf, &x, &y, &z)) {
            if (len == cap) {
                cap *= 2;
                Point *tmp = realloc(pts, cap * sizeof(Point));
                if (!tmp) {
                    free(pts);
                    *out_points = NULL;
                    return 0;
                }
                pts = tmp;
            }
            pts[len].x = x;
            pts[len].y = y;
            pts[len].z = z;
            len++;
        }

        while (*p == '\n' || *p == '\r') p++;
    }

    *out_points = pts;
    return len;
}

static int dsu_find(int *parent, const int x) {
    if (parent[x] != x) {
        parent[x] = dsu_find(parent, parent[x]);
    }
    return parent[x];
}

static int dsu_union_if_diff(int *parent, int *size, const int a, const int b) {
    int ra = dsu_find(parent, a);
    int rb = dsu_find(parent, b);
    if (ra == rb) return 0;
    if (size[ra] < size[rb]) {
        const int tmp = ra;
        ra = rb;
        rb = tmp;
    }
    parent[rb] = ra;
    size[ra] += size[rb];
    return 1;
}

static unsigned long long sq_dist(const Point *p, const Point *q) {
    const long long dx = p->x - q->x;
    const long long dy = p->y - q->y;
    const long long dz = p->z - q->z;
    return (unsigned long long) (dx * dx)
           + (unsigned long long) (dy * dy)
           + (unsigned long long) (dz * dz);
}

static void heap_sift_up(Edge *heap, size_t idx) {
    while (idx > 0) {
        const size_t parent = (idx - 1) / 2;
        if (heap[parent].dist >= heap[idx].dist) break;
        const Edge tmp = heap[parent];
        heap[parent] = heap[idx];
        heap[idx] = tmp;
        idx = parent;
    }
}

static void heap_sift_down(Edge *heap, const size_t heap_size) {
    size_t idx = 0;
    for (;;) {
        const size_t left = idx * 2 + 1;
        const size_t right = idx * 2 + 2;
        size_t largest = idx;

        if (left < heap_size && heap[left].dist > heap[largest].dist) {
            largest = left;
        }
        if (right < heap_size && heap[right].dist > heap[largest].dist) {
            largest = right;
        }
        if (largest == idx) break;

        const Edge tmp = heap[idx];
        heap[idx] = heap[largest];
        heap[largest] = tmp;
        idx = largest;
    }
}

static int edge_cmp(const void *a, const void *b) {
    const Edge *ea = a;
    const Edge *eb = b;
    if (ea->dist < eb->dist) return -1;
    if (ea->dist > eb->dist) return 1;
    return 0;
}

static long long solve_playground_part1(const char *input) {
    Point *pts = NULL;
    const size_t n = parse_points(input, &pts);
    if (!pts || n == 0) {
        free(pts);
        return 0;
    }
    if (n == 1) {
        free(pts);
        return 1;
    }

    const size_t K = 1000;
    Edge *heap = malloc(K * sizeof(Edge));
    if (!heap) {
        free(pts);
        return 0;
    }
    size_t heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            const unsigned long long d = sq_dist(&pts[i], &pts[j]);
            const Edge e = {d, (int) i, (int) j};

            if (heap_size < K) {
                heap[heap_size] = e;
                heap_sift_up(heap, heap_size);
                heap_size++;
            } else if (d < heap[0].dist) {
                heap[0] = e;
                heap_sift_down(heap, heap_size);
            }
        }
    }

    qsort(heap, heap_size, sizeof(Edge), edge_cmp);

    int *parent = malloc(n * sizeof(int));
    int *size = malloc(n * sizeof(int));
    if (!parent || !size) {
        free(parent);
        free(size);
        free(heap);
        free(pts);
        return 0;
    }

    for (size_t i = 0; i < n; ++i) {
        parent[i] = (int) i;
        size[i] = 1;
    }

    for (size_t i = 0; i < heap_size; ++i) {
        dsu_union_if_diff(parent, size, heap[i].a, heap[i].b);
    }

    long long top1 = 0, top2 = 0, top3 = 0;
    for (size_t i = 0; i < n; ++i) {
        if (parent[i] == (int) i) {
            const long long s = size[i];
            if (s > top1) {
                top3 = top2;
                top2 = top1;
                top1 = s;
            } else if (s > top2) {
                top3 = top2;
                top2 = s;
            } else if (s > top3) {
                top3 = s;
            }
        }
    }

    const long long result = top1 * top2 * top3;

    free(parent);
    free(size);
    free(heap);
    free(pts);
    return result;
}

static long long solve_playground_part2(const char *input) {
    Point *pts = NULL;
    const size_t n = parse_points(input, &pts);
    if (!pts || n == 0) {
        free(pts);
        return 0;
    }
    if (n == 1) {
        const long long x = pts[0].x;
        free(pts);
        return x * x;
    }

    unsigned long long *best = malloc(n * sizeof(unsigned long long));
    int *from = malloc(n * sizeof(int));
    bool *in_mst = calloc(n, sizeof(bool));
    if (!best || !from || !in_mst) {
        free(best);
        free(from);
        free(in_mst);
        free(pts);
        return 0;
    }

    for (size_t i = 0; i < n; ++i) {
        best[i] = ULLONG_MAX;
        from[i] = -1;
        in_mst[i] = false;
    }

    const size_t start = 0;
    best[start] = 0;

    unsigned long long max_edge = 0;
    int last_u = -1;
    int last_v = -1;

    for (size_t iter = 0; iter < n; ++iter) {
        size_t u = (size_t) -1;
        unsigned long long min_d = ULLONG_MAX;

        for (size_t i = 0; i < n; ++i) {
            if (!in_mst[i] && best[i] < min_d) {
                min_d = best[i];
                u = i;
            }
        }

        if (u == (size_t) -1) {
            break;
        }

        in_mst[u] = true;

        if (from[u] != -1) {
            if (min_d >= max_edge) {
                max_edge = min_d;
                last_u = from[u];
                last_v = (int) u;
            }
        }

        for (size_t v = 0; v < n; ++v) {
            if (in_mst[v]) continue;
            const unsigned long long d = sq_dist(&pts[u], &pts[v]);
            if (d < best[v]) {
                best[v] = d;
                from[v] = (int) u;
            }
        }
    }

    long long result = 0;
    if (last_u != -1 && last_v != -1) {
        const long long xa = pts[last_u].x;
        const long long xb = pts[last_v].x;
        result = xa * xb;
    }

    free(best);
    free(from);
    free(in_mst);
    free(pts);

    return result;
}

char *solve_part1(const char *input) {
    const long long ans = solve_playground_part1(input);
    return format_string("%lld", ans);
}

char *solve_part2(const char *input) {
    const long long ans = solve_playground_part2(input);
    return format_string("%lld", ans);
}
