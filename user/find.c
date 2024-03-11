#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char *getname(char *path)
{
    char *ptr;
    ptr = path + strlen(path);
    while (ptr >= path && *ptr != '/')
    {
        ptr--;
    }
    ptr++;
    return ptr;
}

void find(char *path, char *str)
{
    char buf[512];
    struct dirent de;
    struct stat st;
    int fd = open(path, 0); // open by standard write mode
    if (!fd)
    {
        // open failed
        return;
    }
    if (!fstat(fd, &st))
    {
        // store file info falied
        close(fd);
        return;
    }
    switch (st.type)
    {
    case T_DEVICE:
        // device file
    case T_FILE:
        // normal file
        if (!strcmp(str, getname(path)))
        {
            printf("%s\n", path); // print the complete path
        }
        break;
    case T_DIR:
        // dictory
        strcpy(buf, path);
        char *p = buf + strlen(path);
        *p = '/';
        p++;
        while (read(fd, &de, sizeof(de)) == sizeof(de)) // read sucess
        {
            if (de.inum == 0)
            {
                // empty dictory or have not used
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (!stat(buf, &st))
            {
                continue;
            }
            if (st.type == T_DEVICE || st.type == T_FILE)
            {
                if (!strcmp(str, getname(path)))
                {
                    printf("%s\n", buf);
                }
            }
            else if (st.type == T_DIR && strcmp(".", getname(buf)) && strcmp("..", getname(buf)))
            {
                find(buf, str);
            }
        }

        break;
    default:
        break;
    }
    close(fd);
    return;
}

int main()
{

    exit(0);
}