#ifndef CMD_UTILS_H
#define CMD_UTILS_H
#include "cmd.h"
#include <string.h>

static inline void cmd_box_clear(cmd_box_t *x) {
    memset(x, 0, sizeof(cmd_box_t));
}
static inline void cmd_prompt_clear(cmd_prompt_t *x) {
    memset(x, 0, sizeof(cmd_prompt_t));
}
static inline void cmd_search_clear(cmd_search_t *x) {
    cmd_box_clear(&x->box);
    cmd_prompt_clear(&x->prompt);
}

#endif /*CMD_UTILS_H*/