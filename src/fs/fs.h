#ifndef FS_H
#define FS_H
#include <stddef.h>

typedef struct abuf_t abuf_t;
typedef struct ctx_t ctx_t;
typedef struct ctx_entries_t ctx_entries_t;
/**
 * @brief get cwd
 * @param ctx 
 * @return 0 on success
 */
int fs_getcwd(ctx_t *ctx);
/**
 * @brief get pretty cwd
 * @param buf 
 * @param bufsize 
 * @param cwd 
 * @return 0 on success
 */
int fs_get_rendered_cwd(char *buf, size_t bufsize, const char *cwd);
/**
 * @brief read dir
 * @param ab OPTIONAL
 * @param p
 * @param path
 * @return 0 on success
 */
int fs_read_dir(abuf_t *ab, ctx_entries_t *p, const char *path);
/**
 * @brief get relative dir
 * @param buf 
 * @param path 
 * @param relative 
 * @return 0 on success, 1 if relative path is not a dir, -1 on fail
 */
int fs_get_relative_dir(char **buf, const char *path, const char *relative);
void fs_clear_entries(ctx_entries_t *p);
#endif /*FS_H*/