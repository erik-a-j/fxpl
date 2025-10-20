#include "ctx/ctx.h"
#include "term/term.h"
#include "fs/fs.h"
#include "fs/ft/magic.h"
#include "abuf/abuf.h"
#include "input/input.h"
#include "output/output.h"
#include "logging/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define INCLUDED_FROM_MAIN
#include "main.h"

int main(int argc, char *argv[]) {
    int term_israw = 0;
    if (term_enable_raw() != 0)
        goto eop;
    term_israw = 1;

    if (ctx_init() != 0)
        goto eop;
    
    if (argc >= 2)
        parse_args(argc, argv);

    ctx_t *ctx = &main_ctx.ctx;
    abuf_t *o_ab = &main_ctx.ctx.o_ab;

    if (term_get_winsize(&ctx->win.rows, &ctx->win.cols) != 0)
        goto eop;
    ctx_update_dims(ctx);

    if (fs_getcwd(ctx) != 0) 
        goto eop;
    if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->CWD) != 0)
        goto eop;
    if (fs_get_relative_dir(&ctx->d_par.path, ctx->CWD, "..") == 0) {
        if (fs_read_dir(&ctx->d_par.ab, &ctx->d_par.e, ctx->d_par.path) != 0)
            goto eop;
    }
    write(STDOUT_FILENO, "\x1b[?25l", 6);

    if (o_refresh(o_ab, ctx, o_ALL) != 0) goto eop;
    
    char *kseq = NULL;
    for (;;) {
        ctx->o_flags = o_NONE;
        int kseq_len = i_read_kseq(&kseq);
        if (term_did_resize()) {
            if (term_get_winsize(&ctx->win.rows, &ctx->win.cols) != 0) {
                break;
            }
            ctx_update_dims(ctx);
            putlog_fmt(LOG_INFO, "did resize, rows: %d, cols: %d", ctx->win.rows, ctx->win.cols);
            if (o_refresh(o_ab, ctx, o_ALL) != 0) {
                putlog(LOG_ERROR, "o_refresh() failed", 0);
                break;
            }
        }
        if (kseq_len == 0) continue;
        
        putlog_fmt(LOG_DEBUG, "%.*s", kseq_len, kseq);

        enum processed ip = i_process_kseq(ctx, kseq, kseq_len);
        if (ip == P_QUIT) break;
        if (o_refresh(o_ab, ctx, ctx->o_flags) != 0) {
            putlog(LOG_ERROR, "o_refresh() failed", 0);
            break;
        }
    }
eop:
    write(STDOUT_FILENO, "\x1b[?25h", 6);
    if (term_israw) term_disable_raw();
    ctx_shutdown();
    return 0;
}