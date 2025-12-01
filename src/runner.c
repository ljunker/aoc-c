#include "aoc_client.h"
#include "solver.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *prog) {
    fprintf(stderr,
            "Usage: %s [--part 1|2] [--submit] [--force]\n"
            "\n"
            "Defaults: --part 1, no --submit, no --force\n"
            "Uses AOC_YEAR=%d, AOC_DAY=%d from the linked day module.\n",
            prog, AOC_YEAR, AOC_DAY);
}

int main(int argc, char **argv) {
    int part = 1;
    int do_submit = 0;
    int force = 0;
    int bench_runs = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--part") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--part requires an argument (1 or 2)\n");
                return 1;
            }
            part = atoi(argv[++i]);
            if (part != 1 && part != 2) {
                fprintf(stderr, "Invalid part: %d (must be 1 or 2)\n", part);
                return 1;
            }
        } else if (strcmp(argv[i], "--submit") == 0 || strcmp(argv[i], "-s") == 0) {
            do_submit = 1;
        } else if (strcmp(argv[i], "--force") == 0) {
            force = 1;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--bench") == 0 && i+1 < argc) {
            bench_runs = atoi(argv[++i]);
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    AocClient client;
    if (aoc_client_init(&client) != 0) {
        fprintf(stderr, "Failed to initialize AoC client.\n");
        return 1;
    }

    char *input = NULL;
    if (aoc_get_input(&client, AOC_YEAR, AOC_DAY, force, &input) != 0) {
        fprintf(stderr, "Error: could not get input for %d day %d.\n", AOC_YEAR, AOC_DAY);
        aoc_client_free(&client);
        return 1;
    }

    char *answer = NULL;
    if (part == 1) {
        answer = solve_part1(input);
    } else {
        answer = solve_part2(input);
    }

    if (!answer) {
        fprintf(stderr, "Solver returned NULL (part %d).\n", part);
        free(input);
        aoc_client_free(&client);
        return 1;
    }
    if (bench_runs == 0) {
        printf("Answer for %d day %d part %d: %s\n", AOC_YEAR, AOC_DAY, part, answer);
    }

    int exit_code = 0;

    if (do_submit) {
        AocSubmissionResult res;
        if (aoc_submit_answer(&client, AOC_YEAR, AOC_DAY, part, answer, &res) != 0) {
            fprintf(stderr, "Error: failed to submit answer.\n");
            exit_code = 1;
        } else {
            printf("\n--- Submission result ---\n%s\n", res.message ? res.message : "(no message)");
            switch (res.status) {
                case AOC_SUBMISSION_CORRECT:         exit_code = 0; break;
                case AOC_SUBMISSION_INCORRECT:       exit_code = 2; break;
                case AOC_SUBMISSION_TOO_RECENT:      exit_code = 3; break;
                case AOC_SUBMISSION_ALREADY_COMPLETED: exit_code = 4; break;
                default:                             exit_code = 1; break;
            }
            aoc_submission_result_free(&res);
        }
    }

    if (bench_runs > 0) {
        benchmark_solver("part1", solve_part1, input, bench_runs);
        benchmark_solver("part2", solve_part2, input, bench_runs);
        return 0;
    }

    free(answer);
    free(input);
    aoc_client_free(&client);
    return exit_code;
}
