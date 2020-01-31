/**
 * \file idlekiller.c
 *
 * Force a program to exit if no i/o happens in $IK_IDLE_TIMEOUT seconds.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>

#ifndef DEFAULT_IDLE_TIMEOUT
//! Default idle timeout if IK_IDLE_TIMEOUT is unspecified.
#define DEFAULT_IDLE_TIMEOUT 10
#endif // DEFAULT_IDLE_TIMEOUT

static time_t last_activity = 0;

/**
 * Wrap the epoll_wait(2) system call. Exit if there has been no i/o
 * within the given interval.
 */
int epoll_wait(int epfd, struct epoll_event *events,
              int maxevents, int timeout) {
    static int (*real_epoll_wait)(int, struct epoll_event *, int, int) = NULL;
    static time_t idle_timeout = 0;

    time_t now;

    if (!real_epoll_wait) {
        real_epoll_wait = dlsym(RTLD_NEXT, "epoll_wait");
        if (getenv("IK_IDLE_TIMEOUT")) {
            idle_timeout = atoi(getenv("IK_IDLE_TIMEOUT"));
        } else {
            idle_timeout = DEFAULT_IDLE_TIMEOUT;
        }
    }

    now = time(NULL);
    if (idle_timeout > 0 && (now - last_activity) > idle_timeout) {
        fprintf(stderr, "! Idle timeout after ~ %d seconds\n", (int)idle_timeout);
        exit(1);
    }

    return real_epoll_wait(epfd, events, maxevents, timeout);
}


/**
 * Wrap the read(2) system call. Update last_activity before calling
 * read().
 */
ssize_t read(int fd, void *buf, size_t count) {
    static ssize_t (*real_read)(int, void *, size_t) = NULL;

    if (!real_read)
        real_read = dlsym(RTLD_NEXT, "read");

    last_activity = time(NULL);
    return real_read(fd, buf, count);
}


/**
 * Wrap the write(2) system call. Update last_activity before calling
 * write().
 */
ssize_t write(int fd, const void *buf, size_t count) {
    static ssize_t (*real_write)(int, void *, size_t) = NULL;

    if (!real_write)
        real_write = dlsym(RTLD_NEXT, "write");

    last_activity = time(NULL);
    return real_write(fd, (void *)buf, count);
}
