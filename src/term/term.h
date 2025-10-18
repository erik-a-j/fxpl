#ifndef TERM_H
#define TERM_H

[[noreturn]] void die(const char *s);
void term_disable_raw();
/**
 * @brief enable raw mode
 * @return 0 on success
 */
int term_enable_raw();
/**
 * @brief check if resized
 * @return 1 on resized, 0 if not
 */
int term_did_resize();
/**
 * @brief get window size
 * @param rows 
 * @param cols 
 * @return 0 on success
 */
int term_get_winsize(int *rows, int *cols);

#endif /*TERM_H*/