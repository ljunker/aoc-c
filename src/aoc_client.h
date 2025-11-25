#ifndef AOC_CLIENT_H
#define AOC_CLIENT_H

#include <stddef.h>

typedef enum {
    AOC_SUBMISSION_CORRECT,
    AOC_SUBMISSION_INCORRECT,
    AOC_SUBMISSION_TOO_RECENT,
    AOC_SUBMISSION_ALREADY_COMPLETED,
    AOC_SUBMISSION_UNKNOWN,
    AOC_SUBMISSION_ERROR
} AocSubmissionStatus;

typedef struct {
    AocSubmissionStatus status;
    char *message;
    char *raw_html;
} AocSubmissionResult;

typedef struct {
    char *session_token;
    char *user_agent;
    char *cache_dir;
} AocClientConfig;

typedef struct {
    AocClientConfig config;
} AocClient;

int aoc_client_init(AocClient *client);
void aoc_client_free(AocClient *client);

/**
 * Fetch puzzle input (possibly from cache).
 * 
 * Returns a heap-allocated string (NUL-terminated) in *out_input.
 * Caller must free(*out_input).
 * 
 * Returns 0 on success, non-zero on error.
 */
int aoc_get_input(AocClient *client, int year, int day, int force, char **out_input);

/**
 * Submit answer and parse response.
 * 
 * Returns 0 on success (HTTP ok, parsed result),
 * non-zero on network/protocol error.
 */
int aoc_submit_answer(AocClient *client,
                      int year,
                      int day,
                      int level,
                      const char *answer,
                      AocSubmissionResult *out_result);

void aoc_submission_result_free(AocSubmissionResult *res);

#endif // AOC_CLIENT_H
