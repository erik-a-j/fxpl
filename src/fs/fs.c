#include "fs.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fs_push_entry(fx_t *fx, const fs_entry_t *e) {
    if (fx->e.num == fx->e.cap) {
        int newcap = fx->e.cap ? fx->e.cap * 2 : 128;
        fs_entry_t *newentries = realloc(fx->e.entries, newcap * sizeof(fs_entry_t));
        if (!newentries) return -1;

        fx->e.cap = newcap;
        fx->e.entries = newentries;
    }
    fx->e.entries[fx->e.num++] = *e;
    return 0;
}

int fs_read_dir(fx_t *fx) {
    DIR *d = opendir(fx->cwd);
    if (!d) return -1;

    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        if (de->d_name[0] == '.' && de->d_name[1] == '\0') continue;
        if (de->d_name[0] == '.' && de->d_name[1] == '.' && de->d_name[2] == '\0') continue;
        fs_entry_t e = {0};
        strncpy(e.name, de->d_name, NAME_MAX);
        e.name[NAME_MAX] = '\0';

        if (fs_push_entry(fx, &e) != 0) return -1;
    }
    closedir(d);
    return 0;
}

int fx_init(fx_t *fx) {
    if (!fx) return -1;
    
    fx->e.num = 0;
    fx->e.cap = 10;
    fx->e.entries = malloc(fx->e.cap * sizeof(fs_entry_t));
    if (!fx->e.entries) return -1;
    memset(fx->cwd, 0, sizeof(fx->cwd));

    return 0;
}

void fx_free(fx_t *fx) {
    if (!fx) return;

    free(fx->e.entries);
    fx->e.cap = 0;
    fx->e.num = 0;
}