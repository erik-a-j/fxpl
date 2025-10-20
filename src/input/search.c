#include "search.h"
#include <ctype.h>
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


int i_search(ctx_t *ctx, const char *kseq, int kseq_len) {
    cmd_box_t *box = &ctx->cmd.search.box;
    cmd_prompt_t *prompt = &ctx->cmd.search.prompt;
    ctx->o_flags = o_NONE;

    if (ctx->cmd.cur_cmd != CMD_SEARCH) {
        cmd_search_clear(&ctx->cmd.search);

        memcpy_strlit(box->text, "find:");
        box->cols = ctx->win.cols / 3;
        box->rows = 3;
        box->s_col = ctx->win.cols / 3;
        box->s_row = 3;
        
        prompt->s_col = box->s_col + 1;
        prompt->s_row = box->s_row + 1;
        prompt->trunc = box->cols - 2;
        prompt->len_text = 0;

        ctx->cmd.box = &ctx->cmd.search.box;
        ctx->cmd.prompt = &ctx->cmd.search.prompt;
        ctx->cmd.cur_cmd = CMD_SEARCH;
        ctx->o_flags = o_BOX | o_PROMPT;
    } else if (ctx->cmd.cur_cmd == CMD_SEARCH) {
        if (kseq_len != 1)
            return 0;

        switch(kseq[0]) {
            case '\x1b':
                ctx->cmd.cur_cmd = CMD_NONE;
                ctx->o_flags = o_ALL | o_e_BOX;
                break;
            case 127:
                if (prompt->len_text > 0) {
                    prompt->text[--prompt->len_text] = '\0';
                    ctx->o_flags = o_BOX | o_PROMPT;
                }
                break;
            default:
                if (prompt->len_text < CMD_PROMPT_T_TEXTSZ) {
                    prompt->text[prompt->len_text++] = kseq[0];
                    ctx->o_flags = o_BOX | o_PROMPT;
                }
                break;
        }
    }
    return 0;
}