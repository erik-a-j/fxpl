#include "term.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static struct termios saved_term;

[[noreturn]] void die(const char *s) {
    if (s) {
        perror(s);
        exit(1);
    }
    exit(0);
}
void term_disable_raw() {
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_term);
}
void term_enable_raw() {
    if (tcgetattr(STDIN_FILENO, &saved_term) == -1) {
        die("tcgetattr");
    }
    atexit(term_disable_raw);
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    struct termios raw = saved_term;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		die("tcsetattr");
	}
}