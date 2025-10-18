#ifndef INPUT_H
#define INPUT_H

enum processed {
    P_FAIL = -1
   ,P_NOOP = 0
   ,P_QUIT
};

typedef struct ctx_t ctx_t;
int i_read_kseq(char **kseq_out);
enum processed i_process_kseq(ctx_t *ctx, const char *kseq, int kseq_len);

#endif /*INPUT_H*/