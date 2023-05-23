#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <arpa/inet.h>

#define BUFSZ 501
#define NUM_EXTENSIONS 6

void logexit (const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
};

int addrparse (const char *addrstr, const char *portstr, struct sockaddr_storage *storage) {
    if (addrstr == NULL || portstr == NULL) return -1;
    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;
    port = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    };

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    };

    return -1;
};

void addrtostr (const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr4->sin_port);
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr, INET6_ADDRSTRLEN + 1)) logexit("ntop");
        port = ntohs(addr6->sin6_port);
    } else {
        logexit("unknown protocol family");
    };

    if (str) snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
};

int server_sockaddr_init (const char *proto, const char *portstr, struct sockaddr_storage *storage) {
    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0) return -1;
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    };
};

int extension_validator (char extension[NUM_EXTENSIONS]) {
    const char *valid_extensions[NUM_EXTENSIONS] = {
        ".cpp", 
        ".c", 
        ".txt", 
        ".tex", 
        ".py", 
        ".java"
    };

    int i = 0;
    while (i < sizeof(valid_extensions) / sizeof(valid_extensions[0])) {
        if (0 == strcmp(extension, valid_extensions[i])) return 1;
        i++;
    };
    return 0;
};

char* read_file (const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("error opening file %s\n", filename);
        return NULL;
    };

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(fileSize + 1);
    if (content == NULL) {
        printf("error allocating memory\n");
        fclose(file);
        return NULL;
    };

    size_t bytesRead = fread(content, sizeof(char), fileSize, file);
    if (bytesRead != (size_t)fileSize) {
        printf("error reading file %s\n", filename);
        fclose(file);
        free(content);
        return NULL;
    };

    size_t filteredSize = 0;
    for (size_t i = 0; i < bytesRead; i++) {
        char c = content[i];
        if (c != '\0' && c != EOF && (c >= 0 && c <= 127)) content[filteredSize++] = c;
    };
    content[filteredSize] = '\0';

    fclose(file);

    return content;
};

char* get_filename(const char* content) {
    const char *valid_extensions[NUM_EXTENSIONS] = {
        ".cpp", 
        ".c", 
        ".txt", 
        ".tex", 
        ".py", 
        ".java"
    };

    const size_t num_extensions = sizeof(valid_extensions) / sizeof(valid_extensions[0]);

    const char* extension_pos = NULL;
    size_t file_length = 0;

    for (size_t i = 0; i < num_extensions; i++) {
        extension_pos = strstr(content, valid_extensions[i]);
        if (extension_pos != NULL) {
            file_length = (size_t)(extension_pos - content) + strlen(valid_extensions[i]);
            break;
        };
    };

    if (extension_pos == NULL) return NULL;
    char* filename = (char*)malloc(file_length + 1);
    if (filename == NULL) return NULL;

    strncpy(filename, content, file_length);
    filename[file_length] = '\0';

    return filename;
};

void delete_dir (const char* path) {
    DIR* dir = opendir(path);
    struct dirent* entry;
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            char file_path[BUFSZ];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
            snprintf(file_path, BUFSZ, "%s/%s", path, entry->d_name);
            remove(file_path);
        }
        closedir(dir);
    };
    remove(path);
};