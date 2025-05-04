#include "http.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

void parse_http_request(const char* buf,
    char* method, size_t mlen,
    char* path, size_t plen,
    char* body, size_t blen)
{
    sscan(buf, "%s %s", method, path);
    const char* bp = strstr(buf, "\r\n\r\n");
    if (bp) {
        strncpy(body, bp + 4, blen - 1);
        body[blen - 1] = '\0';
    }
    else {
        body[0] = '\0';
    }
}

void send_http_response(int client, int code, const char* body) {
    char header[256];
    const char* status = (code == 200 || code == 201) ? "OK" : "Error";
    int len = body ? (int)strlen(body) : 0;

    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "\r\n",
        code, status, len);

    send(client, header, (int)strlen(header), 0);
    if (body) send(client, body, len, 0);
}