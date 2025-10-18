#include "input.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include "../fs/fs.h"
#include "../ctx/ctx.h"
#include "../ctx/ctx_dir_t_utils.h"
#include "../output/o_flags.h"
#include "../logging/log.h"



int i_read_kseq(char **kseq_out) {
    static char kseq[128];
    memset(kseq, 0, sizeof(kseq));
    *kseq_out = NULL;

    struct pollfd pfd = {
        .fd = STDIN_FILENO,
        .events = POLLIN
    };
    
    int pl = poll(&pfd, 1, 16);
    if (pl <= 0) return 0;

    if (pfd.revents & POLLIN) {
        int off = 0;
        for (;;) {
            int n = read(STDIN_FILENO, kseq + off, sizeof(kseq)-1 - off);
            if (n > 0) {
                off += n;
                if (off < (int)sizeof(kseq)-1) break;
                continue;
            }
            if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                break;
            }
            break;
        }
        if (off == 0) return 0;

        kseq[off] = '\0';
        *kseq_out = kseq;
        return off;
    }
    return 0;
}

enum processed i_process_kseq(ctx_t *ctx, const char *kseq, int kseq_len) {
    if (!ctx) return P_FAIL;
    if (!kseq || kseq_len == 0) return P_NOOP;

    if (kseq_len == 1) { switch (kseq[0]) {
        case 'q': /* fall through */ 
        case 'Q': return P_QUIT;
        
        default: return P_NOOP;
    }}

    if (kseq_len == 3 && kseq[1] == '[') { switch (kseq[2]) {
        case 'D':
        {
            char buf[PATH_MAX+1];
            int r = fs_get_relative_dir(buf, sizeof(buf), ctx->cwd, "..");
            if (r) {
                if (r == -1)
                    return P_FAIL;
                break;
            }
            ctx->o_flags |= o_ALL;

            strcpy(ctx->cwd, buf);
            if (chdir(ctx->cwd) != 0)
                return P_FAIL;
            if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->cwd) != 0)
                return P_FAIL;
            if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->cwd) != 0)
                return P_FAIL;

            if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;

            r = fs_get_relative_dir(buf, sizeof(buf), ctx->cwd, "..");
            if (r) {
                if (r == -1)
                    return P_FAIL;
                fs_clear_entries(&ctx->d_par.e);
                break;
            }
            strcpy(ctx->d_par.path, buf);
            if (fs_read_dir(&ctx->d_par.ab, &ctx->d_par.e, ctx->d_par.path) != 0)
                return P_FAIL;
            break;
        }
        case 'C':
        {
            char buf[PATH_MAX+1];
            int r = fs_get_relative_dir(buf, sizeof(buf), ctx->cwd, ctx->d_cur.e.ent[ctx->win.cy].name);
            if (r) {
                if (r == -1) return P_FAIL;
                break;
            }
            ctx_dir_move(&ctx->d_par, &ctx->d_cur);

            strcpy(ctx->cwd, buf);
            if (chdir(ctx->cwd) != 0) return P_FAIL;
            if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->cwd) != 0) {
                return P_FAIL;
            }
            if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->cwd) != 0) return P_FAIL;
            if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;
            ctx->o_flags |= o_ALL;
            break;
        }
        case 'A':
            if (ctx->win.cy == 0) {
                ctx->win.cy = ctx->d_cur.e.num-1;
            } else {
                ctx->win.cy--;
            }
            ctx->o_flags |= o_CWDENT;
            break;
        case 'B':
            if (ctx->win.cy == ctx->d_cur.e.num-1) {
                ctx->win.cy = 0;
            } else {
                ctx->win.cy++;
            }
            ctx->o_flags |= o_CWDENT;
            break;
        default: break;
    }}

    return P_NOOP;
}