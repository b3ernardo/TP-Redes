#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

#define NUM_EXTENSIONS 6

void logexit (const char *msg);

int addrparse (const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr (const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init (const char *proto, const char *portstr, struct sockaddr_storage *storage);

int extension_validator (char extension[NUM_EXTENSIONS]);

char* read_file (const char* filename);

char* get_filename (const char* content);

void delete_dir (const char* path);