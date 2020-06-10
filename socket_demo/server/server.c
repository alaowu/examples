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

static bool g_exit = false;

static void * uds_server_thread(void *arg)
{
    int ret;
    int server_fd, client_fd;
    char *rcv_buff = (char *)arg;
    struct sockaddr_un srv_addr;
    struct sockaddr_un clt_addr;
    socklen_t len;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_fd < 0) {
        printf("creat server communication socket failed!\n");
    }

    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, UNIX_DOMAIN);
    unlink(UNIX_DOMAIN);

    ret = bind(server_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if(ret < 0) {
        printf("bind server_fd to socket failed!\n");
        close(server_fd);
        unlink(UNIX_DOMAIN);
        return NULL;
    }

    ret = listen(server_fd, 1);
    if(ret<0){
        printf("cannot listen server_fd!\n");
        close(server_fd);
        unlink(UNIX_DOMAIN);
        return NULL;
    }

    printf("get in uds_server_thread!\n");

    len = (socklen_t)sizeof(clt_addr);
    while(!g_exit) {

        client_fd = accept(server_fd, (struct sockaddr*)&clt_addr, &len);
        if(client_fd < 0){
            printf("cannot accept requst from client!\n");
            break;
        }

        ret = recv(client_fd, rcv_buff, BUFFER_SIZE, 0);
        if (ret < 0) {
            printf("receive socket data from client failed!\n");
            break;
        }

        if (!strncmp(rcv_buff, SOCKET_STOP, strlen(SOCKET_STOP))) {
            g_exit = true;
        }

        printf("receive data is : %s\n", rcv_buff);

        close(client_fd);
    }

    close(server_fd);
    unlink(UNIX_DOMAIN);

    printf("### uds_server_thread exit ###\n");

    return NULL;
}

int main(int argc, char *argv[])
{
    int ret;
    char data_buf[BUFFER_SIZE];
    pthread_t uds_tid;

    printf("welcome to socket test program!\n");

    memset(data_buf, '\0', BUFFER_SIZE);

    ret = pthread_create(&uds_tid, NULL, uds_server_thread, (void *)data_buf);
    if (ret != 0) {
        printf("uds_server_thread create failed!\n");
    }

    while(!g_exit)
    {
        sleep(1);
        if (!strncmp(data_buf, SOCKET_START, strlen(SOCKET_START))) {
            printf("receive data[%s] from client successfully!\n", data_buf);
            memset(data_buf, '\0', BUFFER_SIZE);
        }
    }

    pthread_join(uds_tid, NULL);

    printf("##### exit server #####\n");

    return 0;
}

