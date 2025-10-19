#include <stdio.h>

int numlen(ssize_t num) {
    int len = 1;
    if (num < 0) len++;
    while (num > 9 || num < -9) {
        num /= 10;
        len++;
    }
    return len;
}

int main() {
    printf("len == %d\n", numlen(1000000));
    return 0;
}
