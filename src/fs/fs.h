#ifndef FS_H
#define FS_H

#include <sys/types.h>
#include <linux/limits.h>

typedef struct fs_entry_t {
    char name[NAME_MAX+1];
    int is_dir;
    off_t size;
    time_t mtime;
    mode_t mode;
} fs_entry_t;

typedef struct fx_t {
    char cwd[PATH_MAX+1];
    struct {
        fs_entry_t *entries;
        int num;
        int cap;
    } e;
} fx_t;

int fs_read_dir(fx_t *fx);
int fx_init(fx_t *fx);

#endif /*FS_H*/