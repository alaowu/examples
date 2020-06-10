#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "common.h"

#define  UNIX_DOMAIN    "/mnt/socket/socket_un.domain"
#define  BUFFER_SIZE    1024

int main(int argc, char *argv[])
{
    int ret;
    int client_fd;
    char send_buf[BUFFER_SIZE];
    struct sockaddr_un srv_addr;

    memset(send_buf, '\0', BUFFER_SIZE);

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(client_fd < 0) {
        printf("creat client communication socket failed!\n");
    }

    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, UNIX_DOMAIN);

    ret = connect(client_fd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    if(ret < 0) {
        printf("connect server failed!\n");
        close(client_fd);
        return -1;
    }

    printf("send data to server: size = %ld cmd = %s\n", strlen(argv[1]), argv[1]);
    memcpy(send_buf, argv[1], strlen(argv[1]));

    ret = send(client_fd, send_buf, strlen(send_buf), 0);
    if (ret < 0) {
        printf("send data to server failed!\n");
        close(client_fd);
        return -1;
    }

    close(client_fd);

    printf("##### exit client #####\n");

    return 0;
}


