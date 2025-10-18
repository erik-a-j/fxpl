#include "term.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static struct termios saved_term;
static int saved_fcntl_fl;
static volatile sig_atomic_t resized = 0;

[[noreturn]] void die(const char *s) {
    if (s) {
        perror(s);
        exit(1);
    }
    exit(0);
}

static void on_winch(int sig) { (void)sig; resized = 1; }
static int signals_init() {
        struct sigaction sa = {0};
        sa.sa_handler = on_winch;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGWINCH, &sa, NULL) == -1) {
            return -1;
        }
        return 0;
}
int term_did_resize() {
    if (resized) {
        resized = 0;
        return 1;
    }
    return 0;
}
int term_get_winsize(int *rows, int *cols) {
    *rows = 0;
    *cols = 0;
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    }
    *rows = ws.ws_row;
    *cols = ws.ws_col;
    return 0;
}
void term_disable_raw() {
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    fcntl(STDIN_FILENO, F_SETFL, saved_fcntl_fl);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_term);
}
int term_enable_raw() {
    if (tcgetattr(STDIN_FILENO, &saved_term) == -1) {
        return -1;
    }
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    struct termios raw = saved_term;
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		return -1;
	}
    saved_fcntl_fl = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (saved_fcntl_fl == -1) return -1;
    if (fcntl(STDIN_FILENO, F_SETFL, saved_fcntl_fl | O_NONBLOCK) == -1) {
        return -1;
    }
    if (signals_init() != 0) {
        return -1;
    }
    return 0;
}