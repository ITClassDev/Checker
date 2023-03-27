#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

int main(){
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[4000];
    json res;
    //char ans[6] = {"B", "U", "F", "F", "E", "R"};


    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) printf("No socket created!");
    puts("OK! Socket created");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5206);

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("Error in binding!");
        return 1;
    }
    puts("Bind ok");
    listen(socket_desc, 1);
    while (1){ //messy
        puts("Waiting for clients to connect...");
        c = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0){
            perror("Can't accept client");
            return 1;
        }
        puts("Connection accepted");
        while ((read_size = recv(client_sock, client_message, 4000, 0)) > 0) {
            //write(client_sock, client_message, strlen(client_message));  // echo back message           
            //write(client_sock, ans, 6)
        }
        res = json::parse((std::string)client_message);
        std::cout << res << "\n";
        close(client_sock);

        if (read_size == 0) {
            puts("Client disconnected!");
            fflush(stdout);
        }else if (read_size == -1)
            perror("Recv failed!");
    }
    return 0;   
}
