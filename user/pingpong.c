#include "kernel/types.h"
#include"kernel/stat.h"
#include "user/user.h"

int main()
{
    int p[2];
    pipe(p);
    int pid = fork();
    if (!pid)
    {
        //when there is a child process
        read(p[0],&pid,4);//read from pipe's read hand
        printf("%d:recieve ping\n",pid);
    }
    else
    {
        write(p[1],&pid,4);
        wait(0);
        printf("%d:recieve pong\n",pid);
    }
    
    exit(0);
}