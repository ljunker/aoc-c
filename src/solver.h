#ifndef SOLVER_H
#define SOLVER_H

/**
 * Implemented in each days/dayXX.c.
 * These identify which AoC puzzle this binary belongs to.
 */
extern const int AOC_YEAR;
extern const int AOC_DAY;

/**
 * Solve part 1.
 * Must return a heap-allocated string (malloc),
 * which the caller will free().
 */
char *solve_part1(const char *input);

/**
 * Solve part 2.
 * Same rule: return malloc'd string.
 */
char *solve_part2(const char *input);

#endif // SOLVER_H
