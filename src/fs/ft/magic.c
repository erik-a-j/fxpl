#include <stdlib.h>
#include <string.h>
#include <magic.h>
#include "ft.h"
#include "../fs_entry_t.h"
#include "../../abuf/abuf.h"
#include "../../logging/log.h"

static magic_t *mgc;

int magic_init(magic_t *cookie) {
    *cookie = magic_open(MAGIC_MIME_TYPE | MAGIC_SYMLINK);
    if (!*cookie) return -1;
    if (magic_load(*cookie, NULL) != 0) {
        magic_close(*cookie);
        return -1;
    }
    mgc = cookie;
    return 0;
}
const char *magic_get_mimetype(const char *path) {
    if (*mgc == NULL) return NULL;

    const char *magic = magic_file(*mgc, path);
    putlog_fmt(LOG_INFO, "%s: %s", path, magic);
    return magic;
}

#define ASSIGN_RETURN(ent, ft_icon_strlit, ft_icon_color_strlit) do {                      \
    if (FS_ENTRY_T_FT_FTSZ < sizeof(ft_icon_strlit) + 2)                                   \
        return;                                                                            \
    if (FS_ENTRY_T_FT_COLORFORMATSZ < sizeof(ft_icon_color_strlit))                        \
        return;                                                                            \
    memcpy(ent->ft, " " ft_icon_strlit " ", 1 + sizeof(ft_icon_strlit) + 1);               \
    memcpy(ent->ft_color, ft_icon_color_strlit, sizeof(ft_icon_color_strlit));             \
    return;                                                                                \
} while (0)
void get_ft(fs_entry_t *e) {
    const char *name = e->name;
    if (e->is_dir) {
        if (name[0] == '.') {
            if (strcmp(name, ".config") == 0)
                ASSIGN_RETURN(e, CFGDIR_FT, DIR_FT_COLOR);
            if (strcmp(name, ".git") == 0)
                ASSIGN_RETURN(e, GITDIR_FT, DIR_FT_COLOR);
        }

        ASSIGN_RETURN(e, DIR_FT, DIR_FT_COLOR);
    }
    const char *mime = ab_view(&e->mime_type);
    if (!mime) return;

  {
    const char *p = mime + sizeof("text/")-1;
    putlog_fmt(LOG_DEBUG, "get_ft: p == %s", p);

    if (strcmp(p, "x-makefile") == 0)
        ASSIGN_RETURN(e, GNU_FT, "");
    
    p = mime + sizeof("application/")-1;

    if (strcmp(p, "json") == 0)
        ASSIGN_RETURN(e, JSON_FT, JSON_FT_COLOR);
  }
    
  {
    if (strcmp(name, ".gitignore") == 0)
        ASSIGN_RETURN(e, GIT_FT, GIT_FT_COLOR);
  }

    ASSIGN_RETURN(e, FILE_FT, "");
}
#undef ASSIGN_RETURN