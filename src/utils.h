#ifndef UTILS_H
#define UTILS_H

#define pfuncerr(fmt, ...) putlog_fmt(LOG_ERROR, "ERROR: %s:%d: "fmt, __func__, __LINE__, ##__VA_ARGS__)

#endif /*UTILS_H*/