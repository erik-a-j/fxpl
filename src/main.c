#include "term/term.h"
#include "fs/fs.h"
#include "input/input.h"

#include <stdio.h>
#include <unistd.h>

int main() {

    fx_t fx;
    if (fx_init(&fx) != 0) return 1;

    if (!getcwd(fx.cwd, sizeof(fx.cwd))) return 1;

    if (fs_read_dir(&fx) != 0) return 1;

    for (int i = 0; i < fx.e.num; i++) {
        printf("%s\n", fx.e.entries[i].name);
    }

    //term_enable_raw();
    //char *kseq = NULL;
    //for (;;) {
    //    if (i_read_kseq(&kseq) == 0) continue;
    //    if (kseq[0] == 'q') break;
    //}
    //term_disable_raw();
    return 0;
}