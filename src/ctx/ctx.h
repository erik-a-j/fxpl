#ifndef CTX_H
#define CTX_H

#define CTX_DIR_T_AB_INITCAP 1024
#define CTX_ENTRIES_T_ENT_INITCAP 16
#define CTX_RCWD_MAX 1024

#include "../abuf/abuf_t.h"
#include "../fs/fs_entry_t.h"
#include "../cmd/cmd.h"

typedef struct ctx_entries_t {
    fs_entry_t *ent; // init size = CTX_ENTRIES_T_ENT_INITCAP * sizeof(fs_entry_t)
    int num;
    int cap;
} ctx_entries_t;

typedef struct ctx_dir_t {
    char *path;
    ctx_entries_t e;
    abuf_t ab;
} ctx_dir_t;

typedef struct ctx_win_t {
    int rows, cols;
    int erows, ecols;
    int erows_beg, ecols_beg;
    int etop;
    int cy, cx;
    int error;
} ctx_win_t;

typedef struct ctx_cmd_t {
    cmd_search_t search;
    cmd_box_t *box;
    cmd_prompt_t *prompt;
} ctx_cmd_t;

typedef struct ctx_t {
    ctx_win_t win;

    #define CWD d_cur.path
    char rcwd[CTX_RCWD_MAX];

    ctx_dir_t d_cur;
    ctx_dir_t d_par;

    ctx_cmd_t cmd;

    abuf_t o_ab;
    int o_flags;
} ctx_t;

#endif /*CTX_H*/