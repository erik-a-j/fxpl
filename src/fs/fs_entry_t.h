#ifndef FS_ENTRY_T_H
#define FS_ENTRY_T_H

#include <sys/types.h>
#include <linux/limits.h>
#include "../abuf/abuf_view_t.h"

#define FS_ENTRY_T_FT_FTSZ 8
#define FS_ENTRY_T_FT_COLORFORMATSZ 20
typedef struct fs_entry_t {
    char name[NAME_MAX+1];
    char ft[FS_ENTRY_T_FT_FTSZ];
    char ft_color[FS_ENTRY_T_FT_COLORFORMATSZ];
    int is_dir;
    off_t size;
    time_t mtime;
    mode_t mode;
    abuf_view_t mime_type;
} fs_entry_t;

#endif /*FS_ENTRY_T_H*/