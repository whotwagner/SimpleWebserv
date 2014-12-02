#ifndef SIMPLE_HTTP_H
#define SIMPLE_HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define MAX_BUF_LEN 1024
#define MAX_METHOD_LEN 10
#define MAX_PATH_LEN 800
#define MAX_HTTPSTR_LEN 12


int parse_first_http(const char* http_req,const int req_len);
int handle_client(int connfd);

#endif
