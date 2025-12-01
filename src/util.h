#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <time.h>

char *str_dup(const char *s);
char *path_join(const char *a, const char *b);
char *format_string(const char *fmt, ...);
int ensure_dir_exists(const char *path);
int read_file_to_string(const char *path, char **out_content);
int write_string_to_file(const char *path, const char *content);

void strip_trailing_newlines(char *s);
void strip_html_tags_inplace(char *s);
void normalize_whitespace_inplace(char *s);

typedef struct {
    const char* p;
} scanner;

void scanner_init(scanner* sc, const char* input);
int scanner_next(scanner* sc, const char* fmt, ...);

double now_sec(void);

void benchmark_solver(
    const char* name,
    char* (*solver_fn)(const char*),
    const char* input,
    int runs
);

#endif // UTIL_H
