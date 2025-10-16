#ifndef TERM_H
#define TERM_H

[[noreturn]] void die(const char *s);
void term_disable_raw();
void term_enable_raw();

#endif /*TERM_H*/