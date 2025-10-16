#include "abuf.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

int ab_init(abuf_t *ab, size_t size) {
    if (!ab) return -1;

    ab->b = malloc(size);
    if (!ab->b) return -1;
    ab->cap = size;
    ab->len = 0;
    return 0;
}

void ab_app(abuf_t *ab, const char *data, size_t size) {

}

void ab_app_fmt(abuf_t *ab, const char *fmt, ...) {
    va_list ap;
    va_start(fmt, ap);
}