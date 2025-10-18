#ifndef MAIN_H
#define MAIN_H



#ifdef INCLUDED_FROM_MAIN
static ctx_t ctx;
#include "ctx/ctx_utils.h"

static inline void ctx_shutdown() {
    o_ab_destroy();
    //ab_destroy(&ctx.d_cur.ab);
    //ab_destroy(&ctx.d_par.ab);
    //ctx_entries_free(&ctx.d_cur.e);
    //ctx_entries_free(&ctx.d_par.e);
    ctx_dir_destroy(&ctx.d_cur);
    ctx_dir_destroy(&ctx.d_par);

    mgc_close();
    log_close();
}
static inline int ctx_init() {
    ctx.win.rows = ctx.win.cols = 0;
    ctx.win.erows = ctx.win.ecols = 0;
    ctx.win.erows_beg = ctx.win.ecols_beg = 0;
    ctx.win.etop = 0;
    ctx.win.cy = ctx.win.cx = 0;
    ctx.win.error = 0;

    memset(ctx.rcwd, 0, sizeof ctx.rcwd);

    if (ctx_dir_init(&ctx.d_cur) != 0) return -1;
    if (ctx_dir_init(&ctx.d_par) != 0) return -1;

    ctx.o_flags = o_NONE;

    return 0;
}
#endif /*INCLUDED_FROM_MAIN*/
#endif /*MAIN_H*/