#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    int p[2];
    pipe(p);
    int pid = fork();
    if (!pid)
    {
        char *child_p = (char *)malloc(5);
        child_p[4] = '\0';
        read(p[0], (void *)child_p, 4);
        char *s = "pong";
        write(p[1], (void *)s, 4);
        printf("child process: %s\n", (char *)child_p);
    }
    else
    {
        char *father_p = "ping";
        write(p[1], (void *)father_p, 4);
        wait(0);
        char *s = (char *)malloc(5);
        read(p[0], (void *)s, 4);
        s[4] = '\0';
        printf("father process: %s\n", (char *)s);
    }
    exit(0);
}
