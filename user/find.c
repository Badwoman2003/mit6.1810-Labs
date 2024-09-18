#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *getFileName(char *path)
{
    char *p;
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

void find(char *path, char *name)
{
    char buff_[512];
    struct dirent de;
    struct stat st;

    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        fprintf(2, "find: can not open %s\n", path);
        return;
    }
    if (fstat(fd, &st))
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        if (!strcmp(name, getFileName(path)))
        {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        strcpy(buff_, path);
        char *p = buff_ + strlen(buff_);
        *p = '/';
        p++;
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (!de.inum)
            {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buff_, &st) < 0)
            {
                printf("find: cannot stat %s\n", buff_);
                continue;
            }
            if (st.type == T_DEVICE || st.type == T_FILE)
            {
                if (!strcmp(name, getFileName(path)))
                {
                    printf("%s\n", buff_);
                }
            }
            else if (st.type == T_DIR && strcmp(".", getFileName(buff_)) && strcmp("..", getFileName(buff_)))
            {
                find(buff_, name);
            }
        }
        break;
    }
    close(fd);
    return;
}

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        find(argv[1], argv[2]);
    }
    else
    {
        printf("arguments invalid\n");
    }
    exit(0);
}
