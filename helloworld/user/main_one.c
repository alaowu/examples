#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hello.h"

#define DEV_NAME   "/dev/demo"

int main(int argc, char **argv)
{
    int fd, ret;
    char tmp;
    bool bexit = false;
    char buf[128];

    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0)
    {
        printf("open %s error\n", DEV_NAME);
        return -1;
    }

    printf("open %s success, fd = %d\n", DEV_NAME, fd);

    while (!bexit)
    {
        tmp = getchar();
        if (tmp == 'q')
        {
            bexit = true;
        }
        else if (tmp == 'c')
        {
            ret = ioctl(fd, IOCTL_DEMO_CMD_CLOSE, NULL);
            printf("ioctl->close, ret: %d\n", ret);
        }
        else if (tmp == 'g')
        {
            ret = ioctl(fd, IOCTL_DEMO_CMD_GET, (unsigned long)buf);
            printf("ioctl->get, ret: %s\n", buf);
        }
        else if (tmp == 'o')
        {
            ret = ioctl(fd, IOCTL_DEMO_CMD_OPEN, NULL);
            printf("ioctl->open, ret: %d\n", ret);
        }
        else if (tmp == 'p')
        {
            memset(buf, 0x0, sizeof(buf));
            strncpy(buf, "put data test", sizeof(buf));
            ret = ioctl(fd, IOCTL_DEMO_CMD_PUT, (unsigned long)buf);
            printf("ioctl->put, ret: %d\n", ret);
        }
    }

    close(fd);

    printf("close %s success, fd = %d\n", DEV_NAME, fd);

    return 0;
}

