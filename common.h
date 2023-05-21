#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

void logexit (const char *msg);

int addrparse (const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr (const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init (const char *proto, const char *portstr, struct sockaddr_storage *storage);

int extension_validator (char extension[6]);

char* read_file (const char* filename);

char* extract_filename (const char* content);

void remove_directory (const char* path);