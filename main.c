#include <netdb.h>
#include <stdio.h>
#include "socket.h"




void *start_routine(int*argv) {
    char hostbuffer[1024], clientbuffer;



}


int main() {

    int socket_file_descriptor = socket_init();

    socket_thread(start_routine);

}
