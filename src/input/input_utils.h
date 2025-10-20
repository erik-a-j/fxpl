#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H
#ifdef __INTELLISENSE__
#  include "input.h"
#  include "../fs/dirname.h"
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <poll.h>
#  include <errno.h>
#  include "../fs/fs.h"
#  include "../ctx/ctx.h"
#  include "../ctx/ctx_utils.h"
#  include "../output/o_flags.h"
#  include "../logging/log.h"
#endif

#if defined(INCLUDED_FROM_INPUT) || defined(__INTELLISENSE__)
//#include "../fs/basename.h"

static inline int i_get_peekdir(ctx_t *ctx) {
    char *buf = NULL;
    int r = fs_get_relative_dir(&buf, ctx->CWD, ctx->d_cur.e.ent[ctx->d_cur.cy].name);
    if (r) {
        if (r == -1)
            return -1;
        return 0;
    }
    ctx->o_flags |= o_PEEKENT;
    ctx_dir_destroy(&ctx->d_peek);
    ctx_dir_init(&ctx->d_peek);

    ctx->d_peek.path = buf;
    buf = NULL;

    if (fs_read_dir(&ctx->d_peek.ab, &ctx->d_peek.e, ctx->d_peek.path) != 0)
        return -1;

    return 0;
}

static inline int i_chdir(ctx_t *ctx, const fs_entry_t *p) {
    char *buf = NULL;

    if (p == NULL) {
        if (ctx->CWD[0] == '/' && ctx->CWD[1] == '\0')
            return 0;
        
        ctx->o_flags |= o_ALL;
        ctx_dir_move(&ctx->d_peek, &ctx->d_cur);
        ctx_dir_move(&ctx->d_cur, &ctx->d_par);

        buf = basename(ctx->d_peek.path);
        if (!buf) return -1;

        for (int i = 0; i < ctx->d_cur.e.num; i++) {
            if (strcmp(buf, ctx->d_cur.e.ent[i].name) == 0) {
                ctx->d_cur.cy = i;
                break;
            }
        }
        buf = NULL;

        ctx_dir_init(&ctx->d_par);

        if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0)
            return -1;

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
    } else if (p->is_dir) {
        ctx->o_flags |= o_ALL;
        ctx_dir_move(&ctx->d_par, &ctx->d_cur);
        ctx_dir_move(&ctx->d_cur, &ctx->d_peek);

        i_get_peekdir(ctx);

        if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0)
            return -1;
    }

    if (chdir(ctx->CWD) != 0)
        return -1;
    
    return 0;
}


#endif /*INCLUDED_FROM_INPUT*/
#endif /*INPUT_UTILS_H*/