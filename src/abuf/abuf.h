#ifndef ABUF_H
#define ABUF_H

typedef struct abuf_t {
    char *b;
    int len;
    int cap;
} abuf_t;

#endif /*ABUF_H*/