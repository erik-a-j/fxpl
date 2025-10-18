#include "../ctx/ctx.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "output.h"
#include "symbols/symbols.h"
#include "../abuf/abuf.h"
#include "../fs/fs.h"
#include "../fs/fs_utils.h"
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

    int rows = win->erows;
    int cols = win->ecols;

    for (int i = 0; i < rows; i++) {
        int scr_y = win->erows_beg+1 + i;
        int scr_x = win->ecols_beg+1;
        int idx = win->etop + i;
        int is_cur_row = (idx == win->cy)? 1 : 0;

        ab_appstrlit("\x1b[0m");
        ab_appfmt(0, "\x1b[%d;%dH", scr_y, scr_x);
        ab_appnch(' ', cols);
        ab_appfmt(0, "\x1b[%dD", cols);

        if (idx > e->num) continue;
        
        if (is_cur_row)
            ab_appstrlit("\x1b[7m");

        if (e->ent[idx].is_dir)
            ab_appstrlit("\x1b[1m\x1b[38;2;233;196;97m");
        ab_appnch(' ', 3);

        const char *name = e->ent[idx].name;
        int name_space = cols - 3;
        if (name_space < 0) name_space = 0;

        ab_appfmt(name_space, "%s", name);

        int name_len = (int)strlen(name);
        if (name_len > name_space) name_len = name_space;
        int pad = cols - 3 - name_len;
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
static inline void draw_status_bar(ctx_t *ctx) {
    fs_entry_t *ent = &ctx->d_cur.e.ent[ctx->win.cy];
    ab_appstrlit("\x1b[0m");
    ab_appfmt(0, "\x1b[%d;0H", ctx->win.rows);
    ab_appstrlit("\x1b[K");
    ab_appstrlit(LHALF_CIRCLE);
    //fs_pretty_size_t ps = fs_get_pretty_size( )
    ab_appfmt(0, "%ld", ent->size);
    ab_appstrlit(RHALF_CIRCLE);
    //ab_appfmt(ctx->win.cols, "%s", ctx->d_cur.e.ent[ctx->win.cy].name);
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
    if (flags & o_STATUS) {
        draw_status_bar(ctx);
        if (ab.error) return -1;
    }
    if (write_all(STDOUT_FILENO, ab.b, ab.len) != 0) return -1;
    return 0;
}