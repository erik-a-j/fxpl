#ifndef FS_DIRNAME_H
#define FS_DIRNAME_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

static inline char *fs_dirname(char *buf, size_t sz, const char *path) {
    if (!buf || sz == 0 || !path) {
        errno = EINVAL;
        return NULL;
    }

    size_t n = strlen(path);
    char *tmp = malloc(n+1);
    if (!tmp) return NULL;

    memcpy(tmp, path, n+1);

    char *d = dirname(tmp);
    size_t need = strlen(d)+1;
    if (need > sz) {
        free(tmp);
        errno = ERANGE;
        return NULL;
    }

    memcpy(buf, d, need);
    free(tmp);
    return buf;
}

/**
 * @brief allocates mem for dirname of path
 * @param path 
 * @return malloced string, NULL on fail
 */
static inline char *fs_dirname_dup(const char *path) {
    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    size_t n = strlen(path);
    char *buf = malloc(n+1);
    if (!buf) return NULL; //errno = ENOMEM

    memcpy(buf, path, n+1);

    char *d = dirname(buf);
    return d;
}

#endif /*FS_DIRNAME_H*/