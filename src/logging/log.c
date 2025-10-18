#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../utils.h"

static char logpath[1024];
static FILE *logfp;
static const char *loglevels[] = {"ERROR","WARN","INFO","DEBUG"};

void putlog(enum log_lvl_t lvl, const char *s, size_t n) {
    int logwasclosed = 0;

    if (!logfp) {
        logfp = fopen(logpath, "w");
        if (!logfp) return;
        logwasclosed = 1;
    }

    time_t now;
    struct tm *t;
    char buf[128];
    time(&now);
    t = localtime(&now);
    size_t logflen = strftime(buf, sizeof(buf), "%H:%M:%S ", t);
    if (logflen > 126 || lvl < LOG_ERROR || lvl > LOG_DEBUG) {
        fwrite("LOGGING ERROR", sizeof("LOGGING ERROR")-1, 1, logfp);
        fflush(logfp);
        return;
    }
    logflen += snprintf(buf + logflen, sizeof(buf) - logflen, "%s: ", loglevels[lvl]);
    fwrite(buf, logflen, 1, logfp);

    size_t s_len = (n)? n : strlen(s);
    fwrite(s, s_len, 1, logfp);
    if (s[s_len - 1] != '\n') {
        fputc('\n', logfp);
    }
    fflush(logfp);

    if (logwasclosed && logfp) fclose(logfp);
}
void putlog_fmt(enum log_lvl_t lvl, const char *fmt, ...) {
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

void log_close() {
    if (logfp) fclose(logfp);
    logfp = NULL; 
}
int log_init(const char *path) {
    if (!path) return -1;

    if (strlen(path) > sizeof(logpath)-1) return -1;

    strcpy(logpath, path);

    logfp = fopen(logpath, "w");
    if (!logfp) return -1;

    //atexit(log_close);
    return 0;
}
