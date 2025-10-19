#include <stdlib.h>
#include <magic.h>
#include "../logging/log.h"

static magic_t mgc;

void mgc_close() {
    if (mgc) magic_close(mgc);
    mgc = NULL;
}
static int mgc_init() {
    mgc = magic_open(MAGIC_MIME_TYPE | MAGIC_SYMLINK);
    if (!mgc) return -1;
    if (magic_load(mgc, NULL) != 0) {
        magic_close(mgc);
        return -1;
    }
    return 0;
}
const char *magic_get_mimetype(const char *path) {
    if (mgc == NULL) {
        if (mgc_init() != 0) return NULL;
    }
    const char *magic = magic_file(mgc, path);
    putlog_fmt(LOG_INFO, "%s: %s", path, magic);
    return magic;
}