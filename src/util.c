#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

char *str_dup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

char *path_join(const char *a, const char *b) {
    if (!a) return str_dup(b);
    if (!b) return str_dup(a);

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    int need_sep = (len_a > 0 && a[len_a - 1] != '/');

    size_t total = len_a + need_sep + len_b + 1;
    char *res = malloc(total);
    if (!res) return NULL;

    strcpy(res, a);
    if (need_sep) strcat(res, "/");
    strcat(res, b);
    return res;
}

char *format_string(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);

    int len = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (len < 0) {
        va_end(args);
        return NULL;
    }

    char *buf = malloc((size_t)len + 1);
    if (!buf) {
        va_end(args);
        return NULL;
    }

    vsnprintf(buf, (size_t)len + 1, fmt, args);
    va_end(args);
    return buf;
}

int ensure_dir_exists(const char *path) {
    if (!path || !*path) return -1;

    if (mkdir(path, 0755) == 0) return 0;
    if (errno == EEXIST) return 0;
    return -1;
}

int read_file_to_string(const char *path, char **out_content) {
    *out_content = NULL;
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }
    long size = ftell(f);
    if (size < 0) {
        fclose(f);
        return -1;
    }
    rewind(f);

    char *buf = malloc((size_t)size + 1);
    if (!buf) {
        fclose(f);
        return -1;
    }

    size_t read = fread(buf, 1, (size_t)size, f);
    fclose(f);
    if (read != (size_t)size) {
        free(buf);
        return -1;
    }
    buf[size] = '\0';
    *out_content = buf;
    return 0;
}

int write_string_to_file(const char *path, const char *content) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, f);
    fclose(f);
    return (written == len) ? 0 : -1;
}

void strip_trailing_newlines(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

void strip_html_tags_inplace(char *s) {
    if (!s) return;
    char *dst = s;
    int in_tag = 0;
    for (char *p = s; *p; p++) {
        if (*p == '<') {
            in_tag = 1;
            continue;
        }
        if (*p == '>') {
            in_tag = 0;
            continue;
        }
        if (!in_tag) {
            *dst++ = *p;
        }
    }
    *dst = '\0';
}

void normalize_whitespace_inplace(char *s) {
    if (!s) return;
    char *dst = s;
    int in_space = 0;
    for (char *p = s; *p; p++) {
        char c = *p;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!in_space) {
                *dst++ = ' ';
                in_space = 1;
            }
        } else {
            *dst++ = c;
            in_space = 0;
        }
    }
    *dst = '\0';
}

void scanner_init(scanner* sc, const char* input) {
    if (!sc) return;
    sc->p = input ? input : "";
}

static int scanner_count_conversions(const char* fmt) {
    int count = 0;
    const char* p = fmt;
    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == '%') {
                p++;
                continue;
            }
            count++;
            while (*p && strchr("diuoxXfFeEgGaAcspn", *p) == NULL) {
                p++;
            }
            if (*p) p++;
        } else {
            p++;
        }
    }
    return count;
}

int scanner_next(scanner* sc, const char* fmt, ...) {
    if (!sc || !sc->p || !fmt) return 0;

    const char* p = sc->p;

    for (;;) {
        while (*p == ' ' || *p == '\t' || *p == '\r') {
            p++;
        }
        if (*p == '\n') {
            p++;
            continue;
        }
        break;
    }

    if (*p == '\0') {
        sc->p = p;
        return 0;
    }

    const char* line_start = p;
    const char* line_end = p;
    while (*line_end && *line_end != '\n' && *line_end != '\r') {
        line_end++;
    }

    size_t line_len = (size_t)(line_end - line_start);
    char* buf = malloc(line_len + 1);
    if (!buf) {
        return 0;
    }
    memcpy(buf, line_start, line_len);
    buf[line_len] = '\0';

    int expected = scanner_count_conversions(fmt);
    if (expected <= 0) {
        free(buf);
        sc->p = (*line_end ? line_end + 1 : line_end);
        return 0;
    }

    va_list args;
    va_start(args, fmt);
    int matched = vsscanf(buf, fmt, args);
    va_end(args);

    free(buf);

    if (*line_end == '\n' || *line_end == '\r') {
        const char* q = line_end;
        while (*q == '\n' || *q == '\r') {
            q++;
        }
        sc->p = q;
    } else {
        sc->p = line_end;
    }

    if (matched == expected) {
        return 1;
    } else {
        return 0;
    }
}

double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}
