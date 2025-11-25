#include "aoc_client.h"
#include "util.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AOC_BASE_URL "https://adventofcode.com"

static size_t write_to_string_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total = size * nmemb;
    char **pstr = (char **)userdata;
    size_t old_len = (*pstr) ? strlen(*pstr) : 0;

    char *new_buf = realloc(*pstr, old_len + total + 1);
    if (!new_buf) return 0; // allocation failure

    memcpy(new_buf + old_len, ptr, total);
    new_buf[old_len + total] = '\0';
    *pstr = new_buf;
    return total;
}

int aoc_client_init(AocClient *client) {
    if (!client) return -1;

    memset(client, 0, sizeof(*client));
    client->config.session_token = str_dup(getenv("AOC_SESSION"));
    if (!client->config.session_token || strlen(client->config.session_token) == 0) {
        fprintf(stderr, "Error: AOC_SESSION environment variable not set.\n");
        return -1;
    }

    client->config.user_agent = str_dup("AdventOfCodeCHelper (local)");
    client->config.cache_dir = str_dup(".aoc_cache");

    if (ensure_dir_exists(client->config.cache_dir) != 0) {
        fprintf(stderr, "Warning: failed to create cache dir %s\n", client->config.cache_dir);
    }

    if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0) {
        fprintf(stderr, "Error: curl_global_init failed.\n");
        return -1;
    }

    return 0;
}

void aoc_client_free(AocClient *client) {
    if (!client) return;
    free(client->config.session_token);
    free(client->config.user_agent);
    free(client->config.cache_dir);
    curl_global_cleanup();
}

static char *build_input_cache_path(AocClient *client, int year, int day) {
    char *year_dir = format_string("%s/%d", client->config.cache_dir, year);
    if (!year_dir) return NULL;
    ensure_dir_exists(year_dir);

    char *filename = format_string("day%02d.txt", day);
    char *full = path_join(year_dir, filename);
    free(year_dir);
    free(filename);
    return full;
}

static char *build_url_input(int year, int day) {
    return format_string("%s/%d/day/%d/input", AOC_BASE_URL, year, day);
}

static char *build_url_answer(int year, int day) {
    return format_string("%s/%d/day/%d/answer", AOC_BASE_URL, year, day);
}

int aoc_get_input(AocClient *client, int year, int day, int force, char **out_input) {
    *out_input = NULL;
    if (!client) return -1;

    char *cache_path = build_input_cache_path(client, year, day);
    if (!cache_path) return -1;

    if (!force) {
        if (read_file_to_string(cache_path, out_input) == 0) {
            free(cache_path);
            return 0; // got from cache
        }
    }

    char *url = build_url_input(year, day);
    if (!url) {
        free(cache_path);
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        free(cache_path);
        free(url);
        return -1;
    }

    char *response = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, client->config.user_agent);

    char cookie_header[4096];
    snprintf(cookie_header, sizeof(cookie_header), "session=%s", client->config.session_token);
    curl_easy_setopt(curl, CURLOPT_COOKIE, cookie_header);

    CURLcode res = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);
    free(url);

    if (res != CURLE_OK || status != 200) {
        fprintf(stderr, "Error: failed to fetch input (curl=%d, HTTP=%ld)\n", res, status);
        free(response);
        free(cache_path);
        return -1;
    }

    strip_trailing_newlines(response);

    if (write_string_to_file(cache_path, response) != 0) {
        fprintf(stderr, "Warning: failed to write cache file %s\n", cache_path);
    }

    *out_input = response;
    free(cache_path);
    return 0;
}

static void parse_submission_html(const char *html,
                                  AocSubmissionStatus *out_status,
                                  char **out_message_text) {
    char *copy = str_dup(html);
    if (!copy) {
        *out_status = AOC_SUBMISSION_ERROR;
        *out_message_text = str_dup("Allocation error");
        return;
    }

    char *start = strcasestr(copy, "<article");
    char *end = NULL;
    if (start) {
        char *gt = strchr(start, '>');
        if (gt) {
            start = gt + 1;
            end = strcasestr(start, "</article>");
        }
    }
    if (start && end && end > start) {
        *end = '\0';
    } else {
        start = copy;
    }

    strip_html_tags_inplace(start);
    normalize_whitespace_inplace(start);

    char *msg = str_dup(start);
    char *lower = str_dup(start);
    for (char *p = lower; *p; p++) {
        if (*p >= 'A' && *p <= 'Z') *p = (char)(*p - 'A' + 'a');
    }

    AocSubmissionStatus status = AOC_SUBMISSION_UNKNOWN;
    if (strstr(lower, "that's the right answer") != NULL) {
        status = AOC_SUBMISSION_CORRECT;
    } else if (strstr(lower, "that's not the right answer") != NULL) {
        status = AOC_SUBMISSION_INCORRECT;
    } else if (strstr(lower, "you gave an answer too recently") != NULL) {
        status = AOC_SUBMISSION_TOO_RECENT;
    } else if (strstr(lower, "you don't seem to be solving the right level") != NULL) {
        status = AOC_SUBMISSION_ALREADY_COMPLETED;
    }

    free(copy);
    free(lower);

    *out_status = status;
    *out_message_text = msg;
}

int aoc_submit_answer(AocClient *client,
                      int year,
                      int day,
                      int level,
                      const char *answer,
                      AocSubmissionResult *out_result) {
    memset(out_result, 0, sizeof(*out_result));
    if (!client || !answer) return -1;

    char *url = build_url_answer(year, day);
    if (!url) return -1;

    CURL *curl = curl_easy_init();
    if (!curl) {
        free(url);
        return -1;
    }

    char *response = NULL;

    char *post_fields = format_string("level=%d&answer=%s", level, answer);
    if (!post_fields) {
        curl_easy_cleanup(curl);
        free(url);
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, client->config.user_agent);

    char cookie_header[4096];
    snprintf(cookie_header, sizeof(cookie_header), "session=%s", client->config.session_token);
    curl_easy_setopt(curl, CURLOPT_COOKIE, cookie_header);

    CURLcode res = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);
    free(url);
    free(post_fields);

    if (res != CURLE_OK || status != 200) {
        fprintf(stderr, "Error: failed to submit answer (curl=%d, HTTP=%ld)\n", res, status);
        free(response);
        out_result->status = AOC_SUBMISSION_ERROR;
        out_result->message = str_dup("Network/HTTP error");
        out_result->raw_html = NULL;
        return -1;
    }

    AocSubmissionStatus status_enum;
    char *message_text = NULL;
    parse_submission_html(response, &status_enum, &message_text);

    out_result->status = status_enum;
    out_result->message = message_text;
    out_result->raw_html = response;
    return 0;
}

void aoc_submission_result_free(AocSubmissionResult *res) {
    if (!res) return;
    free(res->message);
    free(res->raw_html);
    memset(res, 0, sizeof(*res));
}
