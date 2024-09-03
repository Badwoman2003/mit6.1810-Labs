#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int isPrime(int num)
{
    int flag = 1;
    if (num == 2)
    {
        return 1;
    }
    if (num % 2 == 0)
    {
        return 0;
    }
    for (int i = 2; i < num; i++)
    {
        if (num % i == 0)
        {
            return 0;
        }
    }
    return flag;
}
void print_prime(int p[2])
{
    close(p[1]); // close write
    int n;
    read(p[0], &n, 4); // read from p[idx]
    close(p[0]);
    if (isPrime(n))
    {
        printf("%d \n", n);
    }
    int p2[2];
    pipe(p2);
    int pid = fork();
    int next = n + 1;
    if (!pid)
    {
        if (next <= 35)
        {
            print_prime(p2);
        }
        else
        {
            exit(0);
        }
    }
    else
    {
        int next = n + 1;
        if (next <= 35)
        {
            write(p2[1], &next, 4);
            close(p2[1]);
            wait(0);
        }
    }
    exit(0);
}

int main()
{
    int p[2];
    pipe(p);
    int start = 2;
    write(p[1], &start, 4);
    print_prime(p);
    exit(0);
}