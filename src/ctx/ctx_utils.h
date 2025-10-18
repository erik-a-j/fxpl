#ifndef CTX_UTILS_H
#define CTX_UTILS_H
#include "ctx.h"
#include <stdlib.h>
#include <string.h>
#include "../fs/fs_entry_t.h"
#include "../abuf/abuf.h"

static inline void ctx_entries_destroy(ctx_entries_t *x) {
    if (!x) return;
    free(x->ent);
    x->ent = NULL;
    x->cap = 0;
    x->num = 0;
}
static inline void ctx_dir_destroy(ctx_dir_t *x) {
    if (!x) return;
    free(x->path);
    x->path = NULL;
    ctx_entries_destroy(&x->e);
    ab_destroy(&x->ab);
}
static inline int ctx_entries_init(ctx_entries_t *x, size_t sz) {
    if (!x) return -1;
    x->ent = NULL;
    x->cap = 0;
    x->num = 0;
    if (sz == 0) return 0;

    x->ent = malloc(sizeof(fs_entry_t) * sz);
    if (!x->ent) return -1;
    x->cap = (int)sz;
    return 0;
}
static inline int ctx_dir_init(ctx_dir_t *x) {
    if (!x) return -1;

    x->path = NULL;
    if (ctx_entries_init(&x->e, CTX_ENTRIES_T_ENT_INITCAP) != 0) return -1;
    if (ab_init(&x->ab, CTX_DIR_T_AB_INITCAP) != 0) {
        ctx_entries_destroy(&x->e);
        return -1;
    }
    return 0;
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

static inline void ctx_update_dims(ctx_t *x) {
    if (x->win.rows < 0 || x->win.cols < 0) {
        x->win.error = 1;
        return;
    }
    x->win.error     = 0;
    x->win.erows     = x->win.rows - 2;
    x->win.erows_beg = 1;
    x->win.ecols     = x->win.cols * (0.5 * 0.66);
    x->win.ecols_beg = x->win.cols * (0.5 * 0.33);
}


#endif /*CTX_UTILS_H*/