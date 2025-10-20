#ifndef UTILS_H
#define UTILS_H
#include <sys/types.h>

#define pfuncerr(fmt, ...) putlog_fmt(LOG_ERROR, "ERROR: %s:%d: "fmt, __func__, __LINE__, ##__VA_ARGS__)

static inline int numlen(ssize_t num) {
    int len = 1;
    if (num < 0) len++;
    while (num > 9 || num < -9) {
        num /= 10;
        len++;
    }
    return len;
}

#define memcpy_strlit(dest, strlit) memcpy(dest, strlit, sizeof(strlit))

#endif /*UTILS_H*/