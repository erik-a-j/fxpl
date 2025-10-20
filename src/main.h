#ifndef MAIN_H
#define MAIN_H

#ifdef INCLUDED_FROM_MAIN
//static ctx_t ctx;
#include "cmd/cmd.h"
#include "cmd/cmd_utils.h"
#include <magic.h>

static struct main_t {
    ctx_t ctx;
    magic_t _mgc;
    FILE *_logfp;
    FILE *_cwd_file;
} main_ctx;

#include "ctx/ctx_utils.h"

static inline void ctx_shutdown() {
    if (main_ctx._cwd_file) {
        fwrite(main_ctx.ctx.CWD, strlen(main_ctx.ctx.CWD), 1, main_ctx._cwd_file);
        fclose(main_ctx._cwd_file);
        main_ctx._cwd_file = NULL;
    }
    ctx_dir_destroy(&main_ctx.ctx.d_par);
    ctx_dir_destroy(&main_ctx.ctx.d_cur);
    ctx_dir_destroy(&main_ctx.ctx.d_peek);

    ab_destroy(&main_ctx.ctx.o_ab);
    if (main_ctx._mgc) magic_close(main_ctx._mgc);
    main_ctx._mgc = NULL;
    fclose(main_ctx._logfp);
    main_ctx._logfp = NULL;
}
static inline int ctx_init() {
    main_ctx._mgc = NULL;
    main_ctx._logfp = NULL;
    main_ctx._cwd_file = NULL;
    if (ab_init(&main_ctx.ctx.o_ab, 8192) != 0) return -1;
    if (magic_init(&main_ctx._mgc) != 0) return -1;
    if (log_init(&main_ctx._logfp, "/home/arcno/src/fxpl/log.txt") != 0) return -1;

    ctx_t *ctx = &main_ctx.ctx;
    ctx->win.rows = ctx->win.cols = 0;
    ctx->win.erows = ctx->win.ecols = 0;
    ctx->win.erows_beg = ctx->win.ecols_beg = 0;
    ctx->win.etop = 0;
    ctx->win.cy = ctx->win.cx = 0;
    ctx->win.error = 0;

    memset(ctx->rcwd, 0, sizeof ctx->rcwd);

    if (ctx_dir_init(&ctx->d_par) != 0) return -1;
    if (ctx_dir_init(&ctx->d_cur) != 0) return -1;
    if (ctx_dir_init(&ctx->d_peek) != 0) return -1;

    //ctx->cmd.cmd = NULL;
    ctx->cmd.cur_cmd = CMD_NONE;
    cmd_search_clear(&ctx->cmd.search);
    ctx->cmd.box = NULL;
    ctx->cmd.prompt = NULL;

    ctx->o_flags = o_NONE;

    return 0;
}

static inline void parse_args(int argc, char *argv[]) {
    if (argc >= 2) {
        if (strncmp(argv[1], "--cwd-file=", sizeof("--cwd-file=")-1) == 0) {
            const char *cwd_file = argv[1] + sizeof("--cwd-file=")-1;
            main_ctx._cwd_file = fopen(cwd_file, "w");
        }
    }
}

#endif /*INCLUDED_FROM_MAIN*/
#endif /*MAIN_H*/