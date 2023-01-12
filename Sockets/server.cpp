#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 5500
#define MAXBUF  1024

int main() {
    int server_sockfd;
    int client_sockfd;
    int des_fd;
    struct sockaddr_in serveraddr, clientaddr;
    int client_len, read_len, file_read_len;
    char buf[MAXBUF];

    int check_bind;
    client_len = sizeof(clientaddr);

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sockfd == -1) {
        perror("Socket error : ");
        exit(0);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family       = AF_INET;
    serveraddr.sin_addr.s_addr  = htonl(INADDR_ANY);
    serveraddr.sin_port         = htons(PORT);

    if(bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) > 0) {
        perror("Bind error : ");
        exit(0);
    }

    if(listen(server_sockfd, 5) != 0) {
        perror("Listen error : ");
    }

    while(1) {
        char file_name[MAXBUF]; // local val
        memset(buf, 0x00, MAXBUF);

        client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,reinterpret_cast<socklen_t *>(&client_len));
        printf("New Client Connect : %s\n", inet_ntoa(clientaddr.sin_addr));

        read_len = read(client_sockfd, buf, MAXBUF);
        if(read_len > 0) {
            strcpy(file_name, buf);
            printf("%s > %s\n", inet_ntoa(clientaddr.sin_addr), file_name);
        } else {
            close(client_sockfd);
            break;
        }

        des_fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU & (~S_IXUSR));
        if(!des_fd) {
            perror("File open error : ");
            break;
        }

        while(1) {
            memset(buf, 0x00, MAXBUF);
            file_read_len = read(client_sockfd, buf, MAXBUF);
            write(des_fd, buf, file_read_len);
            if(file_read_len == 0) {
                printf("Finish file!!!\n");
                break;
            }

        }

        close(client_sockfd);
        close(des_fd);
    }
    close(server_sockfd);
    return 0;
}