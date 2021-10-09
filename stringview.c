#define _XOPEN_SOURCE 700
#include "stringview.h"
#include <ctype.h>
#include <string.h>
#include <unistd.h>

string_view sv_trim(string_view sv) {
  while (sv.len > 0 && isspace(sv.data[0]))
    sv.data++, sv.len--;
  while (sv.len > 0 && isspace(sv.data[sv.len - 1]))
    sv.len--;
  return sv;
}

int sv_has(string_view sv, const char c) {
  for (size_t i = 0; i<sv.len; i++)
    if (sv.data[i] == c) return 1;
  return 0;
}

string_view sv_split(string_view *sv, string_view delimiters) {
  for(size_t i = 0; i < sv->len; i++) {
    if (sv_has(delimiters, sv->data[i])) {
      const string_view result = {
        .data = sv->data,
        .len = i,
      };
      sv->len -= i + 1;
      sv->data += i + 1;
      return result;
    }
  }
  const string_view result = *sv;
  sv->data = NULL;
  sv->len = 0;
  return result;
}

string_view sv_split_if(string_view *sv, int pred(int)) {
  for(size_t i = 0; i < sv->len; i++) {
    if (pred(sv->data[i])) {
      const string_view result = {
        .data = sv->data,
        .len = i,
      };
      sv->len -= i + 1;
      sv->data += i + 1;
      return result;
    }
  }
  const string_view result = *sv;
  sv->data = NULL;
  sv->len = 0;
  return result;
}


string_view sv_split_escaped(string_view *sv, string_view delimiters) {
  int in_quote = 0;
  int escaped_char = 0;
  for(size_t i = 0; i < sv->len; i++) {
    switch ((in_quote << 1) | escaped_char) {
      case (0 << 1) | 0 : // out of quote, unsecaped
        if (sv_has(delimiters, sv->data[i])) {
          const string_view result = {
            .data = sv->data,
            .len = i,
          };
          sv->len -= i + 1;
          sv->data += i + 1;
          return result;
        }
        if (sv->data[i] == '"') in_quote = 1;
        if (sv->data[i] == '\\') escaped_char = 1;
        break;
      case (1 << 1) | 0 : // in quote, unescaped
        if (sv->data[i] == '"') in_quote = 0;
        break;
      case (0 << 1) | 1 : // out of quote, escaped
        escaped_char = 0;
        break;
      case (1 << 1) | 1 : // in quote, escaped
        escaped_char = 0;
        break;
    }
  }
  const string_view result = *sv;
  sv->data = NULL;
  sv->len = 0;
  return result;
}

string_view sv_from_stream(FILE *stream) {
  char*data = NULL;
  size_t len = 0;
  size_t capacity = 0;
  if (isatty(fileno(stream))) {
    char c;
    while (EOF != (c = getc(stream))) {
      if (capacity < len + 1) {
        capacity = (capacity == 0) ? 1024 : capacity * 2;
        data = (char *)realloc(data, capacity);
        if (!data)
          return (string_view){0};
      }
      data[len++] = c;
    }
  } else {
    if (fseek(stream, 0, SEEK_END) < 0)
      printf("Error: %s\n", strerror(ferror(stream)));

    len = ftell(stream);

    if (fseek(stream, 0, SEEK_SET) < 0)
      printf("Error: %s\n", strerror(ferror(stream)));

    data = (char *)malloc(len);
    fread(data, len, 1, stream);
  }
  return (string_view) {.data = data, .len = len};
}

int sv_len_utf_8(string_view s) {
  int ret = 0;
  for (; s.len > 0; s.len--)
    if (((*s.data++) & 0xc0) != 0x80)
      ret++;
  return ret;
}
