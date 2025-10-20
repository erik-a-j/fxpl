#include "search.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../ctx/ctx.h"
#include "../cmd/cmd_utils.h"
#include "input.h"
#include "../output/output.h"
#include "../output/o_flags.h"
#include "../utils.h"
#include "../logging/log.h"


int i_search(ctx_t *ctx) {
    cmd_search_clear(&ctx->cmd.search);
    cmd_box_t *box = &ctx->cmd.search.box;
    cmd_prompt_t *prompt = &ctx->cmd.search.prompt;

    memcpy_strlit(box->text, "find:");
    box->cols = ctx->win.cols / 3;
    box->rows = 3;
    box->s_col = ctx->win.cols / 3;
    box->s_row = 3;
    
    prompt->s_col = box->s_col + 1;
    prompt->s_row = box->s_row + 1;

    ctx->cmd.box = &ctx->cmd.search.box;
    ctx->cmd.prompt = &ctx->cmd.search.prompt;

    o_refresh(&ctx->o_ab, ctx, o_BOX | o_PROMPT);

    ctx->o_flags = o_NONE;
    return 0;
}