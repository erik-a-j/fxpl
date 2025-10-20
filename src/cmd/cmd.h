#ifndef CMD_H
#define CMD_H

enum cmd_t {
    CMD_NONE
   ,CMD_SEARCH
};

#define CMD_BOX_T_TEXTSZ 64
#define CMD_PROMPT_T_TEXTSZ 256

typedef struct cmd_box_t {
    int s_row, s_col, rows, cols;
    char text[CMD_BOX_T_TEXTSZ];
} cmd_box_t;

typedef struct cmd_prompt_t {
    int s_row, s_col;
    int len_text;
    int trunc;
    char text[CMD_PROMPT_T_TEXTSZ];
} cmd_prompt_t;

typedef struct cmd_search_t {
    cmd_box_t box;
    cmd_prompt_t prompt;
} cmd_search_t;

#endif /*CMD_H*/