#ifndef LOG_H
#define LOG_H
#include <stddef.h>

enum log_lvl_t {
    LOG_ERROR
   ,LOG_WARN
   ,LOG_INFO
   ,LOG_DEBUG
};

typedef struct _IO_FILE FILE;
int log_init(FILE **fp, const char *path);
void putlog(enum log_lvl_t lvl, const char *s, size_t n);
void putlog_fmt(enum log_lvl_t lvl, const char *fmt, ...);

#endif /*LOG_H*/
