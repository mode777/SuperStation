#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{

    int i;
    int fd1, fd2;
    char buf[256];
    char str[100];
    if (argc == 3)
    {
        fd1 = open(argv[1], O_WRONLY);
        fd2 = open(argv[2], O_RDONLY);

        scanf("%s", str);
        write(fd1, str, sizeof(str));
        while (true)
        {
            while (read(fd2, buf, sizeof(buf)) == -1 && errno == EAGAIN) sleep(1);
            read(fd2, buf, sizeof(buf));
            printf("\n%s\n", buf);
        }
        close(fd1);
        close(fd2);
        exit(0);
    }
}