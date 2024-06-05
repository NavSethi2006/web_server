#pragma once
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#define PORT 6969

int socket_init();

void socket_thread();