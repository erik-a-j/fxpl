#ifndef CTX_DIR_T_UTILS_H
#define CTX_DIR_T_UTILS_H
#include "ctx.h"
#include <stdlib.h>
#include <string.h>
#include "../abuf/abuf.h"

static inline void ctx_dir_destroy(ctx_dir_t *x) {
    if (!x) return;
    free(x->e.ent);
    x->e.ent = NULL;
    x->e.cap = 0;
    x->e.num = 0;
    memset(x->path, 0, sizeof x->path);
    ab_destroy(&x->ab);
}

static inline int ctx_dir_move(ctx_dir_t *dst, ctx_dir_t *src) {
    if (!dst || !src) return -1;
    if (dst == src) return 0;

    ctx_dir_t old = *dst;
    *dst = *src;
    memset(src, 0, sizeof *src);
    ctx_dir_destroy(&old);
    return 0;
}

#endif /*CTX_DIR_T_UTILS_H*/