#ifndef PROXY_HH
#define PROXY_HH

#include <sys/queue.h>

char *read_line(int socket);

enum http_methods_enum {
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
    UNKNOWN
};

enum http_versions_enum {
    HTTP_VERSION_1_0,
    HTTP_VERSION_1_1,
    HTTP_VERSION_INVALID
};

typedef struct http_request
{
    enum http_methods_enum method;
    enum http_versions_enum version;
    const char *search_path;

    TAILQ_HEAD(METADATA_HEAD, http_metadata_item) metadata_head;
} http_request;

typedef struct http_metadata_item {
    const char *key;
    const char *value;

    TAILQ_ENTRY(http_metadata_item) entries;
} http_metadata_item;

const char *list_get_key(struct METADATA_HEAD *list, const char *key);
void list_add_key(struct METADATA_HEAD *list, const char *key, const char *value);

#endif