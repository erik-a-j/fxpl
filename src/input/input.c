#include "input.h"
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

int i_read_kseq(char **kseq_out) {
    static char kseq[128];
    memset(kseq, 0, sizeof(kseq));
    *kseq_out = NULL;

    struct pollfd pfd = {
        .fd = STDIN_FILENO,
        .events = POLLIN
    };
    
    int pl = poll(&pfd, 1, 16);
    if (pl <= 0) return 0;

    if (pfd.revents & POLLIN) {
        int off = 0;
        while (off < (int)sizeof(kseq)-1) {
            int n = read(STDIN_FILENO, kseq + off, sizeof(kseq)-1 - off);
            if (n > 0) { off += n; }
            else break;
        }
        if (off == 0) return 0;

        kseq[off] = '\0';
        *kseq_out = kseq;
        return off;
    }
    return 0;
}