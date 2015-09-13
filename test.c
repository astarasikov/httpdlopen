#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "httpdlopen.h"

int make_curl_name(const char* name)
{
    void* raw = httpdlopen_get(name);
    const char* lib = (const char*)raw;
    if (lib == NULL)
        return -1;
    else
        return (int)strlen(lib);
}

int test_curl()
{
    int a, b;

    httpdlopen_set("Makefile", "file://Makefile");

    a = make_curl_name("a");
    b = make_curl_name("b");

    if (a < 100)
        return 1;

    if (b != -1)
        return 1;

    return 0;
}

void* make_dl(const char* name, const char* url, const char* function)
{
    void* lib;
    void* fun;

    lib = dlopen(name, RTLD_NOW);

    if (lib)
    {
        printf("dlopen(%s, RTLD_LAZY) works without httpdlopen_set\n",
               name);
        return NULL;
    }

    httpdlopen_set(name, url);

    lib = dlopen(name, RTLD_LAZY);

    if (!lib)
    {
        printf("dlopen(%s, RTLD_LAZY) does not work "
               "after httpdlopen_set(%s, %s)\n",
               name, name, url);
        return NULL;
    }

    fun = dlsym(lib, function);

    if (!fun)
    {
        printf("dlsym(<POINTER>, %s) does not return function "
               "after httpdlopen_set(%s, %s)"
               " and dlopen(%s, RTLD_LAZY)\n",
               function, name, url, name);
        return NULL;
    }

    return fun;
}

int test_dl()
{
    typedef int (*fun_type)(int);
    fun_type fun;

    fun = (fun_type)make_dl("my name", //"libfibonacci" LIBRARY_SUFFIX,
                            "file://libfibonacci" LIBRARY_SUFFIX,
                            "fibonacci");

    if (!fun)
        return 1;

    if (fun(5) != 8)
    {
        printf("fibonacci function does not work correctly\n");
        return 1;
    }

    return 0;
}

int main(void)
{
    int result_curl;
    int result_dlopen;

    httpdlopen_init();

    result_curl = test_curl();
    result_dlopen = test_dl();

    httpdlopen_deinit();

    if (result_curl != 0)
        return result_curl;

    if (result_dlopen != 0)
        return result_dlopen;

    return 0;
}
