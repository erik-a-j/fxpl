#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>

#define EVENT_SIZE  (sizeof(struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + NAME_MAX + 1))

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_to_watch>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dir = argv[1];
    int fd, wd;
    char buffer[BUF_LEN];

    // Step 1: Initialize inotify
    fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Step 2: Add a watch
    wd = inotify_add_watch(fd, dir, IN_CREATE | IN_DELETE | IN_MODIFY);
    if (wd == -1) {
        perror("inotify_add_watch");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Watching %s for changes...\n", dir);

    // Step 3: Read events in a loop
    while (1) {
        ssize_t length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
            break;
        }

        ssize_t i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            printf("Event: ");

            if (event->mask & IN_CREATE)
                printf("Created: ");
            else if (event->mask & IN_DELETE)
                printf("Deleted: ");
            else if (event->mask & IN_MODIFY)
                printf("Modified: ");

            if (event->len)
                printf("%s\n", event->name);
            else
                printf("(no name)\n");

            i += EVENT_SIZE + event->len;
        }
    }

    // Step 4: Clean up
    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
