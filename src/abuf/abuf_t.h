#ifndef ABUF_T_H
#define ABUF_T_H
#include <stddef.h>

enum abuf_error_t {
    ABNONE = 0
   ,ABNULL
   ,ABMALLOC
   ,ABOVF
   ,ABFMT
};
typedef struct abuf_t {
    char *b;
    size_t len;
    size_t cap;
    enum abuf_error_t error;
} abuf_t;

#endif /*ABUF_T_H*/