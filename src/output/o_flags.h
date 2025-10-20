#ifndef O_FLAGS_H
#define O_FLAGS_H

#define o_NONE    0
#define o_CWD     (1 << 0)
#define o_PARENT  (1 << 1)
#define o_CWDENT  (1 << 2)
#define o_PEEKENT (1 << 3)
#define o_STATUS  (1 << 4)
#define o_ALL     (o_CWD | o_PARENT | o_CWDENT | o_PEEKENT | o_STATUS)
#define o_BOX     (1 << 5)
#define o_e_BOX   (1 << 6)
#define o_PROMPT  (1 << 7)

#endif /*O_FLAGS_H*/