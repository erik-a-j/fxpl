#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
#ifdef __INTELLISENSE__
#  include "input.h"
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <poll.h>
#  include <errno.h>
#  include "../fs/fs.h"
#  include "../fs/dirname.h"
#  include "../ctx/ctx.h"
#  include "../ctx/ctx_utils.h"
#  include "../output/o_flags.h"
#  include "../logging/log.h"
#endif

#if defined(INCLUDED_FROM_INPUT) || defined(__INTELLISENSE__)

static inline int i_chdir(ctx_t *ctx, const char *p) {
    char *buf = NULL;

    if (p == NULL) {
        if (ctx->CWD[0] == '/' && ctx->CWD[1] == '\0')
            return 0;
        
        ctx->o_flags |= o_ALL;
        ctx_dir_move(&ctx->d_cur, &ctx->d_par);
        ctx_dir_init(&ctx->d_par);

        if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0)
            return -1;
        if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;

        int r = fs_get_relative_dir(&buf, ctx->CWD, "..");
        if (r) {
            if (r == -1)
                return -1;
            return 0;
        }
        
        ctx->d_par.path = buf;
        buf = NULL;
        if (fs_read_dir(&ctx->d_par.ab, &ctx->d_par.e, ctx->d_par.path) != 0)
            return -1;
    } else {
        int r = fs_get_relative_dir(&buf, ctx->CWD, p);
        if (r) {
            if (r == -1)
                return -1;
            return 0;
        }
        ctx->o_flags |= o_ALL;
        ctx_dir_move(&ctx->d_par, &ctx->d_cur);
        ctx_dir_init(&ctx->d_cur);
        ctx->d_cur.path = buf;
        buf = NULL;

        if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0)
            return -1;

        if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->CWD) != 0)
            return -1;
        if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;
    }

    if (chdir(ctx->CWD) != 0)
        return -1;
    
    return 0;
}

#endif /*INCLUDED_FROM_INPUT*/
#endif /*INPUT_UTILS_H*/