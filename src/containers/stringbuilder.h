// stringbuilder.h
#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include <stddef.h>

typedef struct {
    char* buf;
    size_t len;
    size_t cap;
} sb;

sb* sb_new(void);
void sb_free(sb* s);

void sb_append(sb* s, const char* text);
void sb_append_char(sb* s, char c);
void sb_append_int(sb* s, long value);

const char* sb_str(sb* s);

#endif
