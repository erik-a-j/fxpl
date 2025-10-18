#ifndef ABUF_VIEW_H
#define ABUF_VIEW_H
#include <stddef.h>

typedef struct abuf_view_t {
    size_t off;
    size_t len;
} abuf_view_t;

#endif /*ABUF_VIEW_H*/