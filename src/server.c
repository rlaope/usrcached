#define _WIN32_WINNT 0x0601
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "http.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT   8080
#define BUF_SZ 4096

DWORD WINAPI client_handler(LPVOID arg) {
    SOCKET client = (SOCKET)(intptr_t)arg;
    char buf[BUF_SZ] = { 0 };

    int r = recv(client, buf, BUF_SZ - 1, 0);
    if (r <= 0) { closesocket(client); return 0; }
    buf[r] = '\0';

    char method[8], path[128], body[256];
    parse_http_request(buf, method, sizeof(method), path, sizeof(path), body, sizeof(body));

    if (strcmp(method, "GET") == 0) {
        char key[128];
        if (sscanf(path, "/cache/%127s", key) == 1) {
            Cache* n = cache_find(key);
            if (n) {
                char res[64];
                snprintf(res, sizeof(res), "{\"user_id\":%d}", n->user_id);
                send_http_response(client, 200, res);
            }
            else {
                send_http_response(client, 404, "{\"error\":\"Not found\"}");
            }
        }
        else {
            send_http_response(client, 404, "{\"error\":\"Invalid path\"}");
        }

    }
    else if (strcmp(method, "POST") == 0) {
        char key[128]; int id;
        if (sscanf(path, "/cache/%127s", key) != 1 || sscanf(body, "user_id=%d", &id) != 1) {
            send_http_response(client, 400, "{\"error\":\"Bad request\"}");
        }
        else {
            int upd = cache_put(key, id);
            send_http_response(client, upd ? 200 : 201,
                upd ? "{\"status\":\"updated\"}" : "{\"status\":\"created\"}");
        }

    }
    else if (strcmp(method, "DELETE") == 0) {
        char key[128];
        if (sscanf(path, "/cache/%127s", key) != 1) {
            send_http_response(client, 400, "{\"error\":\"Bad request\"}");
        }
        else {
            int del = cache_delete(key);
            send_http_response(client, del ? 200 : 404,
                del ? "{\"status\":\"deleted\"}" : "{\"error\":\"Not found\"}");
        }

    }
    else {
        send_http_response(client, 405, "{\"error\":\"Method Not Allowed\"}");
    }

    closesocket(client);
    return 0;
}

int main(void) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }

    cache_init();

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "bind() failed: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    if (listen(listen_sock, 10) == SOCKET_ERROR) {
        fprintf(stderr, "listen() failed: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    printf("Cache server listening on port %d …\n", PORT);
    while (1) {
        SOCKET client = accept(listen_sock, NULL, NULL);
        if (client == INVALID_SOCKET) continue;
        HANDLE h = CreateThread(NULL, 0, client_handler,
            (LPVOID)(intptr_t)client, 0, NULL);
        if (h) CloseHandle(h);
    }

    // 절대 도달하지 않음
    cache_cleanup();
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}