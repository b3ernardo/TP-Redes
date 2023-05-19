#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 501

void usage(int argc, char **argv) {
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
};

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    };

    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    };

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    };

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != connect(s, addr, sizeof(storage))) {
        logexit("connect");
    };

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("connected to %s\n", addrstr);

    FILE *file = NULL;
    char filename[BUFSZ] = "";

    while (1) {
        printf("> ");
        char input[BUFSZ];
        fgets(input, BUFSZ - 1, stdin);
                
        if (0 == strncmp(input, "select file ", strlen("select file "))) {
            if (file != NULL) {
                fclose(file);
            };           
            
            char buf[BUFSZ] = "";
            sscanf(input, "select file %[^\n]", buf);
            file = fopen(buf, "rb"); 

            char *extension = strrchr(buf, '.');
            if (extension == NULL || !extension_validator(extension)) {
                printf("%s not valid!\n", buf);
            } else if (file == NULL) {
                printf("%s does not exist\n", buf);
            } else {
                strcpy(filename, buf);
                file = fopen(filename, "rb"); 
                printf("%s selected\n", buf);
            };
        } else if (0 == strncmp(input, "send file", strlen("send file")) && strlen(input) - 1 == strlen("send file")) {
            if (file == NULL) {
                printf("no file selected!\n");
                continue;
            };

            size_t filename_length = strlen(filename);
            ssize_t sent = send(s, filename, filename_length, 0);
            if (sent == -1) {
                logexit("send");
            };
            
            char recv_buf[BUFSZ];
            memset(recv_buf, 0, BUFSZ);
            ssize_t count = recv(s, recv_buf, BUFSZ - 1, 0);
            if (count == -1) {
                logexit("recv");
            };
        } else if (0 == strncmp(input, "exit", strlen("exit")) && strlen(input) - 1 == strlen("exit")) {
            ssize_t sent = send(s, "exit", strlen("exit"), 0);
            if (sent == -1) {
                logexit("send");
            }
            break;
        } else {
            ssize_t sent = send(s, "unknown", strlen("unknown"), 0);
            if (sent == -1) {
                logexit("send");
            };
            break;
        };
    };
    
    if (file != NULL) {
        fclose(file);
    };

    close(s);
    exit(EXIT_SUCCESS);
}; 