#ifndef OUTPUT_H
#define OUTPUT_H
#include "o_flags.h"

typedef struct ctx_t ctx_t;
int o_refresh(ctx_t *ctx, int flags);
void o_ab_destroy();

#endif /*OUTPUT_H*/