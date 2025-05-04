#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

void parse_http_request(const char* buf,
    char* method, size_t mlen,
    char* path, size_t plen,
    char* body, size_t blen);

void send_http_response(int client, int code, const char* body);

#endif 