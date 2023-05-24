#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILES 100
#define BUFSZ 501

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
};

int main(int argc, char **argv) {
    if (argc < 3) usage(argc, argv);

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) usage(argc, argv);

    int s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) logexit("socket");

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) logexit("setsockopt");

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) logexit("bind");

    char file_list[MAX_FILES][BUFSZ];
    int close_server = 0;
    int file_counter = 0;

    delete_dir("server_files");
    while (1) {    
        if (0 != listen(s, 10)) logexit("listen");

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
            while (1) {
                char caddrstr[BUFSZ];
                addrtostr(caddr, caddrstr, BUFSZ);

                char content[BUFSZ];
                memset(content, 0, BUFSZ);
                ssize_t content_length = recv(csock, content, BUFSZ - 1, 0);
                
                if (0 == strncmp(content, "exit", 4)) {
                    close_server = 1;
                    printf("connection closed\n");
                    break;
                };

                if (0 == strncmp(content, "unknown", strlen("unknown"))) break;

                if (content_length > 0) {
                    char* filename = get_filename(content);
                    const char* folder = "server_files";
                    mkdir(folder, 0700);

                    char filepath[BUFSZ];
                    snprintf(filepath, BUFSZ, "%s/%s", folder, filename);

                    FILE* file = fopen(filepath, "wb");
                    ssize_t bytes_written = fwrite(content, 1, content_length, file);
                    if (bytes_written < content_length) logexit("file");
                    fclose(file);
                    
                    int file_exists = 0;
                    for (int i = 0; i < file_counter; i++) {
                        if (strcmp(content, file_list[i]) == 0) {
                            file_exists = 1;
                            break;
                        };
                    };

                    if (file_exists) {
                        printf("file %s overwritten\n", get_filename(content));
                    } else {
                        strcpy(file_list[file_counter], content);
                        file_counter++;
                        printf("file %s received\n", get_filename(content));
                    };
                } else {
                    printf("error receiving file %s\n", get_filename(content));
                };
                
                ssize_t sent = send(csock, "message received", strlen("message received"), 0);
                if (sent == -1) logexit("send");
            };
            close(csock);
        };
        if (close_server == 1) break;
    };
    exit(EXIT_SUCCESS);
};