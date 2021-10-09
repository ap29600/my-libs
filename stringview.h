#ifndef STRINGVIEW_H_
#define STRINGVIEW_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/// the basic struct

typedef struct {
  size_t len;
  const char *data;
} string_view;

/// utilities

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)(sv).len, (sv).data
#define SV(T) ((string_view) { .len=sizeof(T)-1, .data=(T) })
static inline size_t min(size_t a, size_t b) { return a < b ? a : b; }

/// construct stringviews

string_view sv_from_stream(FILE *stream);
static inline string_view sv_from_cstr(const char *data) {
  return (string_view){
    .len = strlen(data),
    .data = data,
  };
}
static inline string_view sv_from_nstr(const char *data, size_t max_len) {
  return (string_view){
    .len = min(strlen(data), max_len), 
    .data = data,
  };
}

/// slice and dice
string_view sv_trim(string_view sv);
string_view sv_split(string_view *sv, string_view delimiters);
string_view sv_split_escaped(string_view *sv, string_view delimiters);
string_view sv_split_if(string_view *sv, int pred(int));

/// check properties
int sv_has(string_view sv, const char c);
int sv_len_utf_8(string_view s);
static inline int sv_cmp(string_view a, string_view b) {
  return 
    a.len == b.len ? 
    strncmp(a.data, b.data, a.len) : 
    1;
}
static inline int sv_starts_with(string_view a, string_view b) {
  return
    a.len >= b.len ?
    !strncmp(a.data, b.data, b.len) :
    0;
}

#endif // STRINGVIEW_H_

