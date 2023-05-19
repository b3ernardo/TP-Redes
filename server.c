#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 501

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
};

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    };

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    };

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    };

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    };

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    };

    int close_server = 0;
    while (1) {    
        if (0 != listen(s, 10)) {
            logexit("listen");
        };

        char addrstr[BUFSZ];
        addrtostr(addr, addrstr, BUFSZ);
        printf("bound to %s, waiting connections\n", addrstr);

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        } else { 
            while(1) {
                char caddrstr[BUFSZ];
                addrtostr(caddr, caddrstr, BUFSZ);

                char filename[BUFSZ];
                memset(filename, 0, BUFSZ);
                ssize_t filename_length = recv(csock, filename, BUFSZ - 1, 0);

                if (0 == strncmp(filename, "exit", 4)) {
                    ssize_t sent = send(csock, "connection closed", strlen("connection closed"), 0);
                    if (sent == -1) {
                        logexit("exit");
                    };
                    close_server = 1;
                    printf("connection closed\n");
                    break;
                };

                if (0 == strncmp(filename, "unknown", strlen("unknown"))) {
                    ssize_t sent = send(csock, "connection closed", strlen("connection closed"), 0);
                    if (sent == -1) {
                        logexit("unknown");
                    };
                    break;
                };

                if (filename_length > 0) {
                    printf("file %s received\n", filename);
                } else {
                    printf("error receiving file %s\n", filename);
                };
                
                ssize_t sent = send(csock, "message received", strlen("message received"), 0);
                if (sent == -1) {
                    // printf("error sending confirmation\n");
                } else {
                    // printf("confirmation sent to the client\n");
                };
            };

            close(csock);
        };

        if (close_server == 1) break;
    };

    exit(EXIT_SUCCESS);
};