#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../utils.h"

static FILE **logfp;
static const char *loglevels[] = {"ERROR","WARN","INFO","DEBUG"};

void putlog(enum log_lvl_t lvl, const char *s, size_t n) {
    if (!logfp || !*logfp) return;
    if (!s || n == 0) return;

    time_t now;
    struct tm *t;
    char buf[128];
    time(&now);
    t = localtime(&now);

    size_t logflen = strftime(buf, sizeof(buf), "%H:%M:%S ", t);
    if (logflen > 126 || lvl < LOG_ERROR || lvl > LOG_DEBUG) {
        fwrite("LOGGING ERROR", sizeof("LOGGING ERROR")-1, 1, *logfp);
        fflush(*logfp);
        return;
    }
    logflen += snprintf(buf + logflen, sizeof(buf) - logflen, "%s: ", loglevels[lvl]);
    fwrite(buf, logflen, 1, *logfp);

    size_t s_len = (n)? n : strlen(s);
    fwrite(s, s_len, 1, *logfp);
    if (s[s_len - 1] != '\n') {
        fputc('\n', *logfp);
    }
    fflush(*logfp);
}
void putlog_fmt(enum log_lvl_t lvl, const char *fmt, ...) {
    if (!logfp || !*logfp) return;
    if (!fmt) return;

    va_list ap;
    va_start(ap, fmt);

    va_list ap2;
    va_copy(ap2, ap);
    int need_i = vsnprintf(NULL, 0, fmt, ap2);
    va_end(ap2);

    if (need_i < 0 || (size_t)need_i > SIZE_MAX - 1) {
        va_end(ap);
        putlog(LOG_ERROR, "putlog_fmt() ERROR", 0);
        return;
    }

    size_t need = (size_t)need_i;
    char *buf = malloc(need+1);
    if (!buf) {
        va_end(ap);
        putlog(LOG_ERROR, "putlog_fmt() ERROR", 0);
        return;
    }

    int written_i = vsnprintf(buf, need+1, fmt, ap);
    va_end(ap);

    if (written_i < 0 || (size_t)written_i != need) {
        free(buf);
        putlog(LOG_ERROR, "putlog_fmt() ERROR", 0);
        return;
    }

    putlog(lvl, buf, need);
    free(buf);
}

int log_init(FILE **fp, const char *path) {
    if (!fp || !path) return -1;

    *fp = fopen(path, "w");
    if (!*fp) return -1;

    logfp = fp;

    return 0;
}
