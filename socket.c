#include "socket.h"
#include <asm-generic/socket.h>
#include <cstddef>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>

int sockfd;
struct sockaddr_in server_address;
struct sockaddr_in client_address;

int socket_init() {

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sockfd = socket(AF_INET,  SOCK_STREAM, 0);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    if(bind( sockfd, (struct sockaddr*)&server_address,  sizeof(server_address)) < 0) {
        perror("[-] PROBLEM WITH BINDING SOCKET");
        return 1;
    }
    if((listen(sockfd), 10) < 0) {
        perror("[-] PROBLEM WITH LISTENING TO CLIENTS");
        return 1;
    }

    return sockfd;

}


int client;
pthread_t thread;

char hostbuffer[1024];
char servicebuffer[1024];

void socket_thread(void *thread_func(int *arg)) {

    socklen_t client_len = sizeof(client_address);

    while(1) {

        client = accept(sockfd, (struct sockaddr*)&client_address, &client_len);
        if(client < 0) {
            perror("[-]ERROR WITH ACCEPTING");
        }

        getnameinfo((struct sockaddr*)&server_address, sizeof(server_address), hostbuffer, sizeof(hostbuffer), servicebuffer, sizeof(servicebuffer), 0);

        printf("\nServer is listening on http://%s:%s/\n\n", hostbuffer, servicebuffer);

        pthread_create(&thread, NULL, &thread_func, &client);


    }

}
