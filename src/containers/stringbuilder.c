#include "containers/stringbuilder.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define SB_INITIAL_CAP 32

static void sb_ensure_capacity(sb* s, size_t min_cap) {
    if (!s) return;
    if (s->cap >= min_cap) return;

    size_t new_cap = (s->cap > 0) ? s->cap : SB_INITIAL_CAP;
    while (new_cap < min_cap) {
        new_cap *= 2;
    }

    char* new_buf = realloc(s->buf, new_cap * sizeof(char));
    if (!new_buf) {
        fprintf(stderr, "sb_ensure_capacity: out of memory\n");
        abort();
    }

    s->buf = new_buf;
    s->cap = new_cap;
}

sb* sb_new(void) {
    sb* s = malloc(sizeof(sb));
    if (!s) {
        fprintf(stderr, "sb_new: out of memory\n");
        abort();
    }

    s->len = 0;
    s->cap = 0;
    s->buf = NULL;

    sb_ensure_capacity(s, SB_INITIAL_CAP);
    s->buf[0] = '\0';
    return s;
}

void sb_free(sb* s) {
    if (!s) return;
    free(s->buf);
    free(s);
}

void sb_append(sb* s, const char* text) {
    if (!s || !text) return;

    size_t add_len = strlen(text);
    sb_ensure_capacity(s, s->len + add_len + 1);

    memcpy(s->buf + s->len, text, add_len);
    s->len += add_len;
    s->buf[s->len] = '\0';
}

void sb_append_char(sb* s, char c) {
    if (!s) return;

    sb_ensure_capacity(s, s->len + 2);
    s->buf[s->len++] = c;
    s->buf[s->len] = '\0';
}

void sb_append_int(sb* s, long value) {
    if (!s) return;

    char tmp[32];
    int n = snprintf(tmp, sizeof(tmp), "%ld", value);
    if (n < 0) {
        return;
    }
    if ((size_t)n >= sizeof(tmp)) {
        n = (int)(sizeof(tmp) - 1);
        tmp[n] = '\0';
    }
    sb_append(s, tmp);
}

const char* sb_str(sb* s) {
    if (!s) return NULL;
    return s->buf;
}
