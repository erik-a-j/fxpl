#ifndef FS_ENTRY_T_H
#define FS_ENTRY_T_H

#include <sys/types.h>
#include <linux/limits.h>
#include "../abuf/abuf_view_t.h"

typedef struct fs_entry_t {
    char name[NAME_MAX+1];
    char ft[8];
    int is_dir;
    off_t size;
    time_t mtime;
    mode_t mode;
    abuf_view_t mime_type;
} fs_entry_t;

#endif /*FS_ENTRY_T_H*/