#include "../ctx/ctx.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "output.h"
#include "symbols/symbols.h"
#include "colors/fpermissions.h"
#include "../abuf/abuf.h"
#include "../fs/fs.h"
#include "../fs/fs_utils.h"
#include "../fs/ft/ft.h"
#include "../logging/log.h"
#include "../utils.h"

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

#define ab_app(data, size) ab_app(ab, data, (size), NULL)
#define ab_appstrlit(strlit) ab_app(strlit, sizeof(strlit)-1)
#define ab_appfmt(maxsize, ...) ab_appfmt(ab, maxsize, NULL, __VA_ARGS__)
#define ab_appnch(ch, n) ab_appnch(ab, ch, (n))
#define ab_appch(ch) ab_appnch(ch, 1)
#define ab_appnstr(str, len, n) ab_appnstr(ab, str, len, n)
#define ab_appnstrlit(strlit, n) ab_appnstr(strlit, sizeof(strlit)-1, n)

static inline void draw_cwd(abuf_t *ab, ctx_t *ctx) {
    ab_appstrlit("\x1b[H");
    ab_appstrlit("\x1b[K");
    ab_appstrlit("\x1b[38;2;235;219;178m");
    ab_appfmt(ctx->win.cols, "%s", ctx->rcwd);
    ab_appstrlit("\x1b[0m"); 
}
static inline void draw_cwd_entries(abuf_t *ab, ctx_t *ctx) {
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

        if (idx >= e->num) continue;
        
        if (e->ent[idx].is_dir)
            ab_appstrlit("\x1b[1m\x1b[38;2;233;196;97m");

        if (is_cur_row)
            ab_appstrlit("\x1b[7m");

        else if (e->ent[idx].ft_color[0] != '\0')
            ab_app(e->ent[idx].ft_color, strlen(e->ent[idx].ft_color));

        if (e->ent[idx].ft[0] != '\0')
            ab_app(e->ent[idx].ft, strlen(e->ent[idx].ft));

        ab_appstrlit(FT_COLOR_RES);

        if (e->ent[idx].is_dir)
            ab_appstrlit("\x1b[1m\x1b[38;2;233;196;97m");

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
static inline void draw_parent_entries(abuf_t *ab, ctx_t *ctx) {
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
static inline void status_bar_draw_perms(abuf_t *ab, fs_entry_t *ent) {
    if (ent->is_dir) {
        ab_appstrlit(FPERM_DIR_COLOR);
        ab_appch('d');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }

    mode_t mode = ent->mode;
    if (mode & S_IRUSR) {
        ab_appstrlit(FPERM_READ_COLOR);
        ab_appch('r');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IWUSR) {
        ab_appstrlit(FPERM_WRITE_COLOR);
        ab_appch('w');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IXUSR) {
        ab_appstrlit(FPERM_EXEC_COLOR);
        ab_appch('x');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }

    if (mode & S_IRGRP) {
        ab_appstrlit(FPERM_READ_COLOR);
        ab_appch('r');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IWGRP) {
        ab_appstrlit(FPERM_WRITE_COLOR);
        ab_appch('w');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IXGRP) {
        ab_appstrlit(FPERM_EXEC_COLOR);
        ab_appch('x');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }

    if (mode & S_IROTH) {
        ab_appstrlit(FPERM_READ_COLOR);
        ab_appch('r');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IWOTH) {
        ab_appstrlit(FPERM_WRITE_COLOR);
        ab_appch('w');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    if (mode & S_IXOTH) {
        ab_appstrlit(FPERM_EXEC_COLOR);
        ab_appch('x');
    }
    else {
        ab_appstrlit("\x1b[0m");
        ab_appch('-');
    }
    ab_appstrlit("\x1b[0m");
}
static inline void draw_status_bar(abuf_t *ab, ctx_t *ctx) {
    fs_entry_t *ent = &ctx->d_cur.e.ent[ctx->win.cy];
    ab_appstrlit("\x1b[0m");
    ab_appfmt(0, "\x1b[%d;1H", ctx->win.rows);
    ab_appstrlit("\x1b[K");

    int len = 0;

    status_bar_draw_perms(ab, ent);
    ab_appch(' ');
    len += 11;

    {
        ab_appstrlit("\x1b[38;2;255;194;65m");
        ab_appstrlit(LHALF_CIRCLE);
        len++;
    }
    {
        ab_appstrlit("\x1b[7m");
        fs_pretty_size_t ps = fs_get_pretty_size(ent->size);
        ab_appfmt(0, "%ld", ps.size);
        ab_appnch(ps.suffix, 1);
        ab_appstrlit("\x1b[27m");
        len += numlen(ps.size) + 1;
    }
    {
        ab_appstrlit(RHALF_CIRCLE);
        ab_appstrlit("\x1b[0m");
        len++;
    }

    ab_appch(' ');
    len++;

    const char *mime_type = ab_view(&ent->mime_type);
    if (mime_type) {
        ab_appfmt(ctx->win.cols, "%s", mime_type);
        ab_appch(' ');
        len += strlen(mime_type) + 1;
    }

    int e_numlen = numlen(ctx->d_cur.e.num);
    int e_curnumlen = numlen(ctx->win.cy+1);

    int len_left = ctx->win.cols - len;

    if (len_left >= 3 + e_numlen + e_curnumlen) {
        ab_appnch(' ', len_left - (3 + e_numlen + e_curnumlen));
      {
        ab_appstrlit("\x1b[38;2;255;194;65m");
        ab_appstrlit(LHALF_CIRCLE);
      }
      {
        ab_appstrlit("\x1b[7m");
        ab_appfmt(0, "%d/%d", ctx->win.cy+1, ctx->d_cur.e.num);
        ab_appstrlit("\x1b[27m");
      }
      {
        ab_appstrlit(RHALF_CIRCLE);
        ab_appstrlit("\x1b[0m");
      }
    }
}
static inline void draw_box(abuf_t *ab, cmd_box_t *x) {
    if (x->cols < 3 || x->rows < 3) {
        putlog_fmt(LOG_WARN, "draw_box(): dims to small for: %s", x->text);
        return;
    }
    int len_text = (int)strlen(x->text);

    ab_appfmt(0, "\x1b[%d;%dH", x->s_row+1, x->s_col+1);

    ab_appstrlit(TL_ROUND);
  {
    if (len_text <= x->cols-2) {
        ab_app(x->text, len_text);
        ab_appnstrlit(HLINE_THIN, x->cols-2 - len_text);
    } else {
        ab_appnstrlit(HLINE_THIN, x->cols-2);
    }
  }
    ab_appstrlit(TR_ROUND);
  {  
    for (int i = 0; i < x->rows-2; i++)
        ab_appstrlit("\n\b"VLINE_THIN);
  }
    ab_appstrlit("\n\b"BR_ROUND);

    ab_appfmt(0, "\x1b[%d;%dH", x->s_row+1, x->s_col+1+1);
  {  
    for (int i = 0; i < x->rows-2; i++)
        ab_appstrlit("\n\b"VLINE_THIN);
  }
    ab_appstrlit("\n\b"BL_ROUND);
    ab_appnstrlit(HLINE_THIN, x->cols-2);

}
static inline void draw_prompt(abuf_t *ab, cmd_prompt_t *x) {
    ab_appstrlit("\x1b[?25h");
}
int o_refresh(abuf_t *ab, ctx_t *ctx, int flags) {
    if (!ctx) return -1;
    if (ctx->win.error) return 1;

    if (flags == o_NONE) return 0;
    ab_clear(ab);

    if (flags & o_CWD) {
        draw_cwd(ab, ctx);
        if (ab->error) return -1;
    }
    if (flags & o_CWDENT) {
        draw_cwd_entries(ab, ctx);
        if (ab->error) return -1;
    }
    if (flags & o_PARENT) {
        draw_parent_entries(ab, ctx);
        if (ab->error) return -1;
    }
    if (flags & o_STATUS) {
        draw_status_bar(ab, ctx);
        if (ab->error) return -1;
    }
    if (flags & o_BOX && ctx->cmd.box) {
        draw_box(ab, ctx->cmd.box);
        if (ab->error) return -1;
    }
    if (flags & o_PROMPT && ctx->cmd.prompt) {

    }
    if (write_all(STDOUT_FILENO, ab->b, ab->len) != 0) return -1;
    return 0;
}