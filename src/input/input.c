#include "input.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include "../fs/fs.h"
#include "../fs/dirname.h"
#include "../ctx/ctx.h"
#include "../ctx/ctx_utils.h"
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
static void clamp_cursor_and_scroll(ctx_t *ctx) {
    ctx_entries_t *e = &ctx->d_cur.e;
    ctx_win_t *win = &ctx->win;

    int rows = win->erows;
    if (rows < 1) rows = 1;

    if (e->num == 0) { win->cy = 0; win->etop = 0; return; }
    if (win->cy < 0) win->cy = 0;
    if (win->cy >= e->num) win->cy = e->num-1;

    if (win->cy < win->etop) {
        win->etop = win->cy;
    } else if (win->cy >= win->etop + rows) {
        win->etop = win->cy - rows+1;
    }

    int max_top = (e->num > rows)? (e->num - rows) : 0;
    if (win->etop < 0) win->etop = 0;
    if (win->etop > max_top) win->etop = max_top;
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
            char *buf = NULL;
            int r = fs_get_relative_dir(&buf, ctx->CWD, "..");
            if (r) {
                if (r == -1)
                    return P_FAIL;
                break;
            }
            ctx->o_flags |= o_ALL;

            free(ctx->CWD);
            ctx->CWD = buf;
            buf = NULL;
            if (chdir(ctx->CWD) != 0)
                return P_FAIL;
            if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0)
                return P_FAIL;
            if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->CWD) != 0)
                return P_FAIL;

            if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;

            r = fs_get_relative_dir(&buf, ctx->CWD, "..");
            if (r) {
                if (r == -1)
                    return P_FAIL;
                fs_clear_entries(&ctx->d_par.e);
                break;
            }
            
            free(ctx->d_par.path);
            ctx->d_par.path = buf;
            buf = NULL;
            if (fs_read_dir(&ctx->d_par.ab, &ctx->d_par.e, ctx->d_par.path) != 0)
                return P_FAIL;
            break;
        }
        case 'C':
        {
            char *buf = NULL;
            int r = fs_get_relative_dir(&buf, ctx->CWD, ctx->d_cur.e.ent[ctx->win.cy].name);
            if (r) {
                if (r == -1) return P_FAIL;
                break;
            }
            ctx->o_flags |= o_ALL;

            ctx_dir_move(&ctx->d_par, &ctx->d_cur);
            ctx_dir_init(&ctx->d_cur);
            ctx->d_cur.path = buf;
            buf = NULL;
            
            if (chdir(ctx->CWD) != 0) return P_FAIL;
            if (fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->CWD) != 0) {
                return P_FAIL;
            }
            if (fs_read_dir(&ctx->d_cur.ab, &ctx->d_cur.e, ctx->CWD) != 0) return P_FAIL;
            if (ctx->win.cy >= ctx->d_cur.e.num) ctx->win.cy = ctx->d_cur.e.num-1;
            break;
        }
        case 'A':
            if (ctx->win.cy == 0) {
                ctx->win.cy = ctx->d_cur.e.num-1;
            } else {
                ctx->win.cy--;
            }
            clamp_cursor_and_scroll(ctx);
            ctx->o_flags |= (o_CWDENT | o_STATUS);
            break;
        case 'B':
            if (ctx->win.cy == ctx->d_cur.e.num-1) {
                ctx->win.cy = 0;
            } else {
                ctx->win.cy++;
            }
            clamp_cursor_and_scroll(ctx);
            ctx->o_flags |= (o_CWDENT | o_STATUS);
            break;
        default: break;
    }}

    return P_NOOP;
}