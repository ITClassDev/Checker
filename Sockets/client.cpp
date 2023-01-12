#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT    5500
#define IP      "127.0.0.1"
#define MAXBUF  1024

int main() {
    struct sockaddr_in serv_addr;
    int     s;
    int     sourse_fd;
    char    buf[MAXBUF];
    int     filename_len, read_len;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) {
        return 1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(PORT);

    if(connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Connect : ");
        printf("Fail to connect.\n");
        close(s);
        return 1;
    }

    memset(buf, 0x00, MAXBUF);
    printf("Stepik, write filename:  ");
    scanf("%s", buf);

    printf(" > %s\n", buf);
    filename_len = strlen(buf)+1;

    send(s, buf, filename_len, 0);
    sourse_fd = open(buf, O_RDONLY);
    if(!sourse_fd) {
        perror("Error : ");
        return 1;
    }

    while(1) {
        memset(buf, 0x00, MAXBUF);
        read_len = read(sourse_fd, buf, MAXBUF);
        send(s, buf, read_len, 0);
        if(read_len == 0) {
            break;
        }

    }

    return 0;
}