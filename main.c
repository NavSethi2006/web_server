#include <netdb.h>
#include <stdio.h>
#include "socket.h"

int main() {

    int socket_file_descriptor = socket_init();

    socket_thread();

}
