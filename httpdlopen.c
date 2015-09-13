#include <curl/curl.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "httpdlopen.h"

typedef struct
{
    char *name;
    char *url;
    void *data;
    size_t size;
} Library;

static void library_ctor(Library* library,
                         const char* name,
                         const char* url)
{
    library->name = strdup(name);
    library->url = strdup(url);
    library->data = malloc(1);
    library->size = 0;
}

static void library_dtor(Library* library)
{
    free(library->name);
    free(library->url);
    free(library->data);
}

static size_t append_callback(void *data,
                              size_t block_size,
                              size_t block_count,
                              void *library_pointer)
{
    size_t append_size = block_size * block_count;
    Library *library = (Library *)library_pointer;
    size_t new_size = library->size + append_size + 1;

    library->data = realloc(library->data, new_size);
    if(library->data == NULL)
    {
        fprintf(stderr, "can not allocate %lu bytes for %s",
                new_size, library->url);
        return 0;
    }

    memcpy(&(library->data[library->size]), data, append_size);
    library->size += append_size;

    return append_size;
}

static int library_download(Library *library)
{
    CURL *curl_handle;
    CURLcode res;
    int result = 0;

    if (library->size != 0)
        return result;

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, library->url);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, append_callback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)library);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        free(library->data);
        library->data = malloc(1);
        library->size = 0;
        result = 1;
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);

    return result;
}

struct Node
{
    struct Node* next;
    Library library;
};
typedef struct Node Node;

static Node* table;

static Node* table_node_find(const char* name)
{
    Node* node = table;
    while (1)
    {
        if (node == NULL)
            return NULL;
        if (0 == strcmp(name, node->library.name))
            return node;
        node = node->next;
    }
}

static Node* table_node_insert()
{
    Node** result = &table;
    while (*result != NULL)
        result = &((*result)->next);
    *result = (Node*)malloc(sizeof(Node));
    (*result)->next = NULL;
    return *result;
}

void httpdlopen_set(const char* name, const char* url)
{
    Node* node = table_node_find(name);
    if (node == NULL)
        node = table_node_insert();
    else
        library_dtor(&(node->library));
    library_ctor(&(node->library), name, url);
}

void* httpdlopen_get(const char* name)
{
    Library* result = NULL;
    Node* node = table_node_find(name);
    if (node == NULL)
        return NULL;
    result = &node->library;
    if (result->size != 0)
        return result->data;
    if (library_download(result) != 0)
        return NULL;
    return result->data;
}

void* (*default_dlopen)(const char*, int) = NULL;

void httpdlopen_init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    default_dlopen = dlsym(RTLD_NEXT, "dlopen");
}

void httpdlopen_deinit()
{
    while(table != NULL)
    {
        Node* previous = table;
        table = table->next;
        library_dtor(&(previous->library));
        free(previous);
    }
    table = NULL;

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
}

void* dlopen(const char* path, int mode)
{
    void* loaded;
    if (!default_dlopen)
    {
        fprintf(stderr, "can not find default_dlopen, forgot call httpdlopen_init?");
        return NULL;
    }
    loaded = httpdlopen_get(path);
    if (loaded)
        return loaded;
    else
        return (*default_dlopen)(path, mode);
}
