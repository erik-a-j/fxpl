#include "../ctx/ctx.h"
#include "output.h"
#include "symbols/symbols.h"
#include "../abuf/abuf.h"
#include "../fs/fs.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../logging/log.h"
#include "../utils.h"

static abuf_t ab;
void o_ab_destroy() { ab_destroy(&ab); }

static int write_all(int fd, const void *buf, size_t len) {
    if (!buf) return -1;
    if (len == 0) return 0;

    const char *p = buf;
    while (len > 0) {
        ssize_t n = write(fd, p, len);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        p += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

#define ab_app(data, size) ab_app(&ab, data, (size), NULL)
#define ab_appstrlit(strlit) ab_app(strlit, sizeof(strlit)-1)
#define ab_appfmt(maxsize, ...) ab_appfmt(&ab, maxsize, NULL, __VA_ARGS__)
#define ab_appnch(ch, n) ab_appnch(&ab, ch, (n))
#define ab_appnstr(str, len, n) ab_appnstr(&ab, str, len, n)
#define ab_appnstrlit(strlit, n) ab_appnstr(strlit, sizeof(strlit)-1, n)

static inline void draw_cwd(ctx_t *ctx) {
    ab_appstrlit("\x1b[H");
    ab_appstrlit("\x1b[K");
    ab_appstrlit("\x1b[38;2;235;219;178m");
    ab_appfmt(ctx->win.cols, "%s", ctx->rcwd);
    ab_appstrlit("\x1b[0m");
}
static inline void draw_cwd_entries(ctx_t *ctx) {
    ctx_entries_t *e = &ctx->d_cur.e;
    ctx_win_t *win = &ctx->win;
    for (int i = 0; i < win->erows - win->erows_beg; i++) {
        ab_appstrlit("\x1b[0m");
        ab_appfmt(0, "\x1b[%d;%dH", win->erows_beg+1 + i, win->ecols_beg+1);
        ab_appnch(' ', win->ecols);
        if (i >= e->num) continue;
        
        ab_appfmt(0, "\x1b[%dD", win->ecols);
        //ab_appstrlit("\x1b[K");
        if (win->cy == i)
            ab_appstrlit("\x1b[7m");
        if (e->ent[i].is_dir)
            ab_appstrlit("\x1b[1m\x1b[38;2;233;196;97m");
        ab_appnch(' ', 1);
        ab_appnch(' ', 1);

        ab_appfmt(win->ecols, " %s", e->ent[i].name);
        int pad = win->ecols - (int)strlen(e->ent[i].name)-3;
        if (pad > 0) ab_appnch(' ', pad);
    }
}
static inline void draw_parent_entries(ctx_t *ctx) {
    ctx_entries_t *e = &ctx->d_par.e;
    ctx_win_t *win = &ctx->win;
    ab_appstrlit("\x1b[0m");
    for (int i = 0; i < win->erows - win->erows_beg; i++) {
        ab_appfmt(0, "\x1b[%d;0H", win->erows_beg+1 + i);
        ab_appnch(' ', win->ecols_beg);
        if (i >= e->num) continue;

        ab_appfmt(0, "\x1b[%dD", win->ecols_beg);
        if (e->ent[i].is_dir)
            ab_appstrlit("\x1b[1m\x1b[38;2;233;196;97m");
        ab_appfmt(win->ecols_beg, " %s", e->ent[i].name);
        int pad = win->ecols_beg - (int)strlen(e->ent[i].name)-3;
        if (pad > 0) ab_appnch(' ', pad);
        ab_appstrlit("\x1b[0m");
    }
}
int o_refresh(ctx_t *ctx, int flags) {
    if (!ctx) return -1;
    if (ctx->win.error) return 1;
    if (ab.cap == 0) {
        if (ab_init(&ab, 1024) != 0) return -1;
    }
    if (flags == o_NONE) return 0;
    ab_clear(&ab);
    //if (flags & o_ALL) o_clear();

    if (flags & o_CWD) {
        draw_cwd(ctx);
        if (ab.error) return -1;
    }
    if (flags & o_CWDENT) {
        draw_cwd_entries(ctx);
        if (ab.error) return -1;
    }
    if (flags & o_PARENT) {
        draw_parent_entries(ctx);
        if (ab.error) return -1;
    }
    if (write_all(STDOUT_FILENO, ab.b, ab.len) != 0) return -1;
    return 0;
}