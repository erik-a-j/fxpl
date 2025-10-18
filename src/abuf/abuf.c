#include "abuf.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include "../logging/log.h"
#include "../utils.h"

static int add_ovf_size(size_t a, size_t b, size_t *out) {
    if (SIZE_MAX - a < b) return 1;
    *out = a + b;
    return 0;
}
void ab_ensure(abuf_t *ab, size_t size) {
    if (!ab || ab->error || size == 0) return;
    if (ab->cap >= size) return;

    size_t newcap = ab->cap ? ab->cap : 128;
    while (newcap < size) {
        if (newcap > SIZE_MAX / 2) {
            pfuncerr("ABOVF");
            ab->error = ABOVF;
            return;
        }
        newcap <<= 1;
    }

    char *newb = realloc(ab->b, newcap);
    if (!newb) {
        pfuncerr("ABMALLOC");
        ab->error = ABMALLOC;
        return;
    }

    ab->b = newb;
    ab->cap = newcap;
}
const char *ab_view(const abuf_t *ab, const abuf_view_t *view) {
    if (!ab || ab->error) return NULL;
    if (ab->len >= view->off + view->len) {
        return ab->b + view->off;
    }
    return NULL;
}
void ab_app(abuf_t *ab, const char *data, size_t size, abuf_view_t *view) {
    if (!ab || ab->error || size == 0) return;
    if (!data) {
        pfuncerr("ABNULL");
        ab->error = ABNULL;
        return;
    }

    if (view) size++;
    size_t need;
    if (add_ovf_size(ab->len, size, &need)) {
        pfuncerr("ABOVF");
        ab->error = ABOVF;
        return;
    }

    ab_ensure(ab, need);
    memcpy(ab->b + ab->len, data, size);
    if (view) {
        ab->b[ab->len + size-1] = '\0';
        view->off = ab->len;
        view->len = size;
    }
    ab->len += size;
}
void ab_appfmt(abuf_t *ab, size_t maxsize, abuf_view_t *view, const char *fmt, ...) {
    if (!ab || ab->error) return;
    if (!fmt) {
        ab->error = ABNULL;
        return;
    }

    va_list ap;
    va_start(ap, fmt);
    
    va_list ap2;
    va_copy(ap2, ap);
    int need_i = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);

    if (need_i < 0 || (size_t)need_i > SIZE_MAX - 1) {
        va_end(ap);
        pfuncerr("ABFMT");
        ab->error = ABFMT;
        return;
    }
    size_t need = (maxsize > 0 && maxsize < (size_t)need_i)? maxsize : (size_t)need_i;
    char *buf = malloc(need+1);
    if (!buf) {
        va_end(ap);
        pfuncerr("ABMALLOC");
        ab->error = ABMALLOC;
        return;
    }

    int written_i = vsnprintf(buf, need+1, fmt, ap);
    va_end(ap);

    if (written_i < 0) {
        free(buf);
        pfuncerr("ABFMT");
        ab->error = ABFMT;
        return;
    }

    ab_app(ab, buf, need, view);
    free(buf);
}
void ab_appnch(abuf_t *ab, char ch, size_t n) {
    if (!ab || ab->error || n == 0) return;

    size_t need;
    if (add_ovf_size(ab->len, n, &need)) {
        pfuncerr("ABOVF");
        ab->error = ABOVF;
        return;
    }
    ab_ensure(ab, need);
    for (size_t i = 0; i < n; i++) {
        ab->b[ab->len++] = ch;
    }
}
void ab_appnstr(abuf_t *ab, const char *str, size_t len, size_t n) {
    if (!ab || ab->error) return;
    if (len == 0 || n == 0) return;
    if (!str) {
        pfuncerr("ABNULL");
        ab->error = ABNULL;
        return;
    }
    
    size_t need;
    if (add_ovf_size(ab->len, len * n, &need)) {
        pfuncerr("ABOVF");
        ab->error = ABOVF;
        return;
    }
    ab_ensure(ab, need);
    for (size_t i = 0; i < n; i++) {
        memcpy(ab->b + ab->len, str, len);
        ab->len += len;
    }
}
void ab_clear(abuf_t *ab) {
    if (!ab) return;
    if (ab->len > 0) {
        memset(ab->b, 0, ab->len);
        ab->len = 0;
    }
}
int ab_init(abuf_t *ab, size_t size) {
    if (!ab || size == 0) return -1;
    ab->b = NULL;
    ab->cap = 0;
    ab->len = 0;
    ab->error = ABNONE;

    ab->b = malloc(size);
    if (!ab->b) return -1;

    memset(ab->b, 0, size);
    ab->cap = size;

    return 0;
}
void ab_destroy(abuf_t *ab) {
    if (!ab) return;
    free(ab->b);
    ab->b = NULL;
    ab->cap = 0;
    ab->len = 0;
    ab->error = ABNONE;
}
