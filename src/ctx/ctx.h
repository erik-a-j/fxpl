#ifndef CTX_H
#define CTX_H

#define CTX_DIR_T_AB_INITCAP 1024
#define CTX_ENTRIES_T_ENT_INITCAP 16
#define CTX_RCWD_MAX 1024

#include "../abuf/abuf_t.h"
#include "../fs/fs_entry_t.h"

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

typedef struct ctx_t {
    ctx_win_t win;

    //char *cwd;
    char rcwd[CTX_RCWD_MAX];

#define CWD d_cur.path
    ctx_dir_t d_cur;
    ctx_dir_t d_par;
    /*
    ctx_entries_t e;
    struct ctx_parent_t {
        char path[PATH_MAX+1];
        ctx_entries_t e;
    } parent;
    
    abuf_t ab;*/
    int o_flags;
} ctx_t;

#endif /*CTX_H*/