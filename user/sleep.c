#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("arugments wrong\n");
    }
    else
    {
        char *p = argv[1];
        int flag = 1;
        while (*p != '\0')
        {
            if (*p < '0' || *p > '9')
            {
                printf("invaild ticks\n");
                flag = 0;
                break;
            }
            p++;
        }
        if (flag)
        {
            sleep(atoi(argv[1]));
        }
    }
    exit(0);
}