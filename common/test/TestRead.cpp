#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

int main(void)
{
    int fd = open("in2", O_RDONLY);
    char buf[10];
    memset(buf, 0, 10);
    int ret = read(fd, buf, 10);
    for (int  i = 0; i < 10; ++i)
        if (buf[i] == 4)
            puts("get eof");
    return 0;
}
