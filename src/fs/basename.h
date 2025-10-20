#ifndef BASENAME_H
#define BASENAME_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
//#include <libgen.h>
#include <errno.h>

static inline char *fs_basename_dup(const char *path) {
    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    size_t n = strlen(path);
    char *buf = malloc(n+1);
    if (!buf) return NULL; //errno = ENOMEM

    memcpy(buf, path, n+1);

    char *d = basename(buf);
    return d;
}

#endif /*BASENAME_H*/