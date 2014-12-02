#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define MAX_BUF_LEN 10

int handle_client(int connfd);

#endif
