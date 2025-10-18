#include "fs.h"
#include "magic.h"
#include "dirname.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../ctx/ctx.h"
#include "../abuf/abuf.h"
#include "../logging/log.h"
#include "../output/symbols/ft.h"
#include "../utils.h"

static int cmp_dirs_first_name(const void *aa, const void *bb) {
    const fs_entry_t *a = (const fs_entry_t *)aa;
    const fs_entry_t *b = (const fs_entry_t *)bb;
    if (a->is_dir != b->is_dir) return b->is_dir - a->is_dir;
    return strcasecmp(a->name, b->name);
}
static int push_entry(ctx_entries_t *p, const fs_entry_t *e) {
    if (p->num == p->cap) {
        int newcap = p->cap ? p->cap * 2 : 128;
        fs_entry_t *newentries = realloc(p->ent, newcap * sizeof(fs_entry_t));
        if (!newentries) return -1;

        p->cap = newcap;
        p->ent = newentries;
    }
    p->ent[p->num++] = *e;
    return 0;
}
int fs_get_rendered_cwd(char *buf, size_t bufsize, const char *cwd) {
    if (!buf || bufsize == 0 || !cwd) return -1;
    char *home = getenv("HOME");
    if (!strstr(cwd, home)) {
        memcpy(buf, cwd, bufsize);
    } else {
        snprintf(buf, bufsize, "~%s", cwd + strlen(home));
    }
    return 0;
}

int fs_getcwd(ctx_t *ctx) {
    if (!getcwd(ctx->cwd, sizeof(ctx->cwd))) return -1;
    fs_get_rendered_cwd(ctx->rcwd, sizeof(ctx->rcwd), ctx->cwd);
    return 0;
}
void fs_clear_entries(ctx_entries_t *p) {
    if (!p) return;
    for (int i = 0; i < p->num; i++) {
        fs_entry_t *e = &p->ent[i];
        memset(e->ft, 0, sizeof(e->ft));
        memset(e->name, 0, sizeof(e->name));
        e->mime_type.len = 0;
        e->mime_type.off = 0;
        e->is_dir = 0;
        e->mode = 0;
        e->mtime = 0;
        e->size = 0;
    }
    p->num = 0;
}
int fs_read_dir(abuf_t *ab, ctx_entries_t *p, const char *path) {
    if (!p || !path) return -1;
    if (ab) ab_clear(ab);
    fs_clear_entries(p);

    DIR *d = opendir(path);
    if (!d) return -1;
    
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        if (de->d_name[0] == '.' && de->d_name[1] == '\0') continue;
        if (de->d_name[0] == '.' && de->d_name[1] == '.' && de->d_name[2] == '\0') continue;
        fs_entry_t e = {0};
        strncpy(e.name, de->d_name, NAME_MAX);
        e.name[NAME_MAX] = '\0';

        struct stat st;
        char abspath[PATH_MAX+257];
        snprintf(abspath, sizeof(abspath), "%s/%s", path, de->d_name);
        if (lstat(abspath, &st) != 0) {
            e.is_dir = 0;
            e.size = 0;
            e.mtime = 0;
            e.mode = 0;
        } else {
            e.is_dir = S_ISDIR(st.st_mode);
            e.size = st.st_size;
            e.mtime = st.st_mtime;
            e.mode = st.st_mode;
        }
        if (ab) {
            const char *mimetype = magic_get_mimetype(abspath);
            if (mimetype) {
                ab_app(ab, mimetype, strlen(mimetype), &e.mime_type);
                if (ab->error) return -1;
            }
        }
        if (push_entry(p, &e) != 0) return -1;
    }
    closedir(d);

    qsort(p->ent, p->num, sizeof(fs_entry_t), cmp_dirs_first_name);

    return 0;
}

int fs_get_relative_dir(char *buf, size_t bufsize, const char *path, const char *relative) {
    if (!buf || bufsize == 0 || !path || !relative) return -1;

    char newpath[PATH_MAX+2];
    char *p = &newpath[0];

    if (strcmp("..", relative) == 0) {
        
        if (path[0] == '/' && path[1] == '\0') return 1;
        char *mbuf = fs_dirname_dup(path);
        if (!mbuf) {
            pfuncerr("errno == %d", errno);
            return -1;
        }
        strcpy(newpath, mbuf);
        free(mbuf);
        
    } else {
        size_t len_cwd = strnlen(path, PATH_MAX);
        size_t len_rel = strnlen(relative, PATH_MAX);
        if (len_cwd + 1 + len_rel >= sizeof(newpath)) {
            return -1;
        }
        snprintf(newpath, sizeof(newpath), "%s/%s", path, relative);
        if (newpath[0] == '/' && newpath[1] == '/')
            p++;
        DIR *d = opendir(p);
        if (d)
            closedir(d);
        else
            return 1;
    }
    strncpy(buf, p, bufsize);
    //if (chdir(ctx->cwd) != 0) return 0;
    //putlog_fmt(LOG_DEBUG, "chdir to: %s", ctx->cwd);
    //fs_get_rendered_cwd(ctx);
    return 0;
} 

