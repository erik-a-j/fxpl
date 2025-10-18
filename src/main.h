#ifndef MAIN_H
#define MAIN_H



#ifdef INCLUDED_FROM_MAIN
static ctx_t ctx;

static inline void ctx_update_dims() {
    if (ctx.win.rows < 0 || ctx.win.cols < 0) {
        ctx.win.error = 1;
        return;
    }
    ctx.win.error     = 0;
    ctx.win.erows     = ctx.win.rows - 2;
    ctx.win.erows_beg = 1;
    ctx.win.ecols     = ctx.win.cols * (0.5 * 0.66);
    ctx.win.ecols_beg = ctx.win.cols * (0.5 * 0.33);
}
static inline void ctx_entries_free(ctx_entries_t *e) {
    free(e->ent);
    e->ent = NULL;
    e->cap = 0;
    e->num = 0;
}
static inline void ctx_shutdown() {
    o_ab_destroy();
    ab_destroy(&ctx.d_cur.ab);
    ab_destroy(&ctx.d_par.ab);

    ctx_entries_free(&ctx.d_cur.e);
    ctx_entries_free(&ctx.d_par.e);

    mgc_close();
    log_close();
}
static inline int ctx_init() {
    ctx.win.rows = ctx.win.cols = 0;
    ctx.win.erows = ctx.win.ecols = 0;
    ctx.win.erows_beg = ctx.win.ecols_beg = 0;
    ctx.win.cy = ctx.win.cx = 0;
    ctx.win.error = 0;

    ctx.d_cur.ab.b     = ctx.d_par.ab.b = NULL;
    ctx.d_cur.ab.cap   = ctx.d_par.ab.cap = 0;
    ctx.d_cur.ab.len   = ctx.d_par.ab.len = 0;
    ctx.d_cur.ab.error = ctx.d_par.ab.error = 0;
    ctx.d_cur.e.ent = ctx.d_par.e.ent = NULL;
    ctx.d_cur.e.cap = ctx.d_par.e.cap = 0;
    ctx.d_cur.e.num = ctx.d_par.e.num = 0;
    memset(ctx.d_cur.path, 0, sizeof(ctx.d_cur.path));
    memset(ctx.d_par.path, 0, sizeof(ctx.d_par.path));

    memset(ctx.cwd, 0, sizeof(ctx.cwd));
    memset(ctx.rcwd, 0, sizeof(ctx.rcwd));

    ctx.d_cur.e.ent = malloc(sizeof(fs_entry_t) * CTX_ENTRIES_T_ENT_INITCAP);
    if (!ctx.d_cur.e.ent) return -1;
    ctx.d_cur.e.cap = CTX_ENTRIES_T_ENT_INITCAP;
    
    ctx.d_par.e.ent = malloc(sizeof(fs_entry_t) * CTX_ENTRIES_T_ENT_INITCAP);
    if (!ctx.d_par.e.ent) return -1;
    ctx.d_par.e.cap = CTX_ENTRIES_T_ENT_INITCAP;

    ctx.o_flags = o_NONE;
    if (ab_init(&ctx.d_cur.ab, 1024) != 0) return -1;
    if (ab_init(&ctx.d_par.ab, 1024) != 0) return -1;

    return 0;
}
#endif /*INCLUDED_FROM_MAIN*/
#endif /*MAIN_H*/