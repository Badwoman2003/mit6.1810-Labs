#include "kernel/types.h"
#include"kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc != 2) // only when argc equals to 2 we allow it to sleep
    {
        printf("please check argument\n");
    }
    else
    {
        char *p = argv[1];
        int flag = 1;
        while (*p)
        {
            if (*p < '0' || *p > '9')
            {
                flag = 0;
                break;
            }
            p++;
        }
        if (flag)
        {
            int time = atoi(argv[1]);
            sleep(time);
        }
        else
        {
            printf("please check time number\n");
        }
    }
    exit(0);
}
