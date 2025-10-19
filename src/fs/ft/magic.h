#ifndef FS_MAGIC_H
#define FS_MAGIC_H

typedef struct fs_entry_t fs_entry_t;
typedef struct magic_set *magic_t;
int magic_init(magic_t *cookie);
const char *magic_get_mimetype(const char *path);
void get_ft(fs_entry_t *e);

#endif /*FS_MAGIC_H*/