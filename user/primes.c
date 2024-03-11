#include"kernel/types.h"
#include"user/user.h"

void get_prime(int p1[2])
{
    close(p1[1]);   //close write hand
    int n;
    int tag = read(p1[0],&n,4);     //read from pipe,about 4 bytes
    if (!tag)
    {
        //there's no data
        close(p1[0]);
        exit(0);
    }
    printf("prime %d\n",n);
    int p2[2];
    pipe(p2);
    int pid = fork();
    if (pid>0)
    {
        int m;
        while (read(p1[0],&m,4))
        {
            if (m%n)
            {
                write(p2[1],&m,4);
            }
        }
        close(p1[0]);
        close(p2[1]);
        close(p2[0]);
        wait(0);
    }
    else if (!pid)
    {
        //there's a child process
        get_prime(p2);
    }
    else
    {
        exit(pid);
    }
    exit(0);
}

int main()
{
    int p[2];
    pipe(p);
    int i;
    for ( i = 2; i <= 35; i++)
    {
        write(p[1],&i,4);
    }
    get_prime(p);
    exit(0);
}
