#ifndef ABUF_H
#define ABUF_H
#include <stddef.h>
#include "abuf_t.h"
#include "abuf_view_t.h"

const char *ab_view(const abuf_view_t *view);
void ab_ensure(abuf_t *ab, size_t size);
void ab_app(abuf_t *ab, const char *data, size_t size, abuf_view_t *view);
void ab_appfmt(abuf_t *ab, size_t maxsize, abuf_view_t *view, const char *fmt, ...);
void ab_appnch(abuf_t *ab, char ch, size_t n);
void ab_appnstr(abuf_t *ab, const char *str, size_t len, size_t n);
void ab_clear(abuf_t *ab);
/**
 * @brief init abuf
 * @param ab   address of allocated abuf_t struct
 * @param size bytes to reserve
 * @return 0 on success
 */
int ab_init(abuf_t *ab, size_t size);
void ab_destroy(abuf_t *ab);
#endif /*ABUF_H*/