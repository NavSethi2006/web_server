#include "socket.h"
#include <asm-generic/socket.h>

#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>

#define SIZE 1024
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
    if(listen(sockfd, 10) < 0) {
        perror("[-] PROBLEM WITH LISTENING TO CLIENTS");
        return 1;
    }

    return sockfd;

}

void getTimeString(char *buf)
{
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
}

int client;
char hostbuffer[1024];
char servicebuffer[1024];

void get_file_url(char *route, char *get_file_url) {
    char *question = strrchr(route, '?');
    if (question)
        *question = '\0';

    if (route[strlen(route) - 1] == '/')
    {
        strcat(route, "index.html");
    }

    strcpy(get_file_url, "htdocs");
    strcat(get_file_url, route);

    const char *dot = strrchr(get_file_url, '.');
    if (!dot || dot == get_file_url)
    {
        strcat(get_file_url, ".html");
    }
}

void getMimeType(char *file, char *mime)
{
  const char *dot = strrchr(file, '.');

  if (dot == NULL)
    strcpy(mime, "text/html");

  else if (strcmp(dot, ".html") == 0)
    strcpy(mime, "text/html");

  else if (strcmp(dot, ".css") == 0)
    strcpy(mime, "text/css");

  else if (strcmp(dot, ".js") == 0)
    strcpy(mime, "application/js");

  else if (strcmp(dot, ".jpg") == 0)
    strcpy(mime, "image/jpeg");

  else if (strcmp(dot, ".png") == 0)
    strcpy(mime, "image/png");

  else if (strcmp(dot, ".gif") == 0)
    strcpy(mime, "image/gif");

  else
    strcpy(mime, "text/html");
}


void socket_thread() {

    socklen_t client_len = sizeof(client_address);

    getnameinfo((struct sockaddr*)&server_address, sizeof(server_address), hostbuffer, sizeof(hostbuffer), servicebuffer, sizeof(servicebuffer), 0);

    printf("\nServer is listening on http://%s:%s/\n\n", hostbuffer, servicebuffer);

    char *request = (char *)malloc(1024 * sizeof(char));
    char method[10], route[100];

    while(1) {

        client = accept(sockfd, (struct sockaddr*)&client_address, &client_len);
        if(client < 0) {
            perror("[-]ERROR WITH ACCEPTING");
        }

        read(client, request, 1024);

        sscanf(request, "%s %s", method, route);
        printf("%s %s", method, route);
        
        free(request);
        
        char fileURL[100];

        get_file_url(route, fileURL);

        FILE *file = fopen(fileURL, "r");

        if(!file) {
            const char respond[] = "HTTP/1.1 404 Not Found \r\n\n";
            send(client, respond, sizeof(respond), 0);
        }
        char resHeader[SIZE];

        char mimeType[32];
        getMimeType(fileURL, mimeType);

        char timeBuf[100];
        getTimeString(timeBuf);

        sprintf(resHeader, "HTTP/1.1 200 OK\r\nDate: %s\r\nContent-Type: %s\r\n\n", timeBuf, mimeType);
        int headerSize = strlen(resHeader);

        printf(" %s", mimeType);

        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *resBuffer = (char *)malloc(fsize + headerSize);
        strcpy(resBuffer, resHeader);
        char *fileBuffer = resBuffer + headerSize;
        fread(fileBuffer, fsize, 1, file);

        send(client, resBuffer, fsize + headerSize, 0);
        free(resBuffer);
        fclose(file);

    }

}
