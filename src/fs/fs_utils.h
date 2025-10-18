#ifndef FS_UTILS_H
#define FS_UTILS_H
#include <sys/types.h>

typedef struct fs_pretty_size_t {
    off_t size;
    char suffix;
} fs_pretty_size_t;

fs_pretty_size_t fs_get_pretty_size(off_t size) {
    const char *suffixes = "BKMGT";
    int i = 0;
    while (size >= 1024 && i < 4) {
        size /= 1024;
        i++;
    }
    fs_pretty_size_t ps = {.size = size, .suffix = suffixes[i]};
    return ps;
}

#endif /*FS_UTILS_H*/