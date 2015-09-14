#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "httpdlopen.h"

#define LIBRARY_SUFFIX ".so"

int make_curl_name(const char* name)
{
    void* raw = httpdlopen_get(name, NULL);
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

    if (b != -1) {
        fprintf(stderr, "b != -1\n");
        return 1;
    }

    return 0;
}

#define D fprintf(stderr, "%s:%d\n", __func__, __LINE__);

void* make_dl(const char* name, const char* url, const char* function)
{
    void* lib;
    void* fun;

    lib = dlopen(name, RTLD_LAZY);
    D;

    if (lib)
    {
        printf("dlopen(%s, RTLD_LAZY) works without httpdlopen_set\n",
               name);
        return NULL;
    }
    D;

    httpdlopen_set(name, url);
    D;

    lib = dlopen(name, RTLD_LAZY);
    D;

    if (!lib)
    {
        printf("dlopen(%s, RTLD_LAZY) does not work "
               "after httpdlopen_set(%s, %s)\n",
               name, name, url);
        return NULL;
    }
    D;

    fun = dlsym(lib, function);
    D;

    if (!fun)
    {
        printf("dlsym(<POINTER>, %s) does not return function "
               "after httpdlopen_set(%s, %s)"
               " and dlopen(%s, RTLD_LAZY)\n",
               function, name, url, name);
        return NULL;
    }
    D;

    return fun;
}

int test_dl()
{
    typedef int (*fun_type)(int);
    fun_type fun;

    char pathBuf[512] = {};
    char pwdBuf[256] = {};
    getcwd(pwdBuf, 255);
    sprintf(pathBuf, "file://%s/lib/libfibonacci" LIBRARY_SUFFIX,
            pwdBuf);

    fprintf(stderr, "%s\n", __func__);

    fun = (fun_type)make_dl("libfibonacci" LIBRARY_SUFFIX,
                            pathBuf,
                            "fibonacci");

    if (!fun) {
        fprintf(stderr, "failed to find function!\n");
        return 1;
    }
    else {
        fprintf(stderr, "fibonacci fun ptr=%p\n", fun);
    }

    if (fun(5) != 8)
    {
        fprintf(stderr, "fibonacci function does not work correctly\n");
        return 1;
    }
    fprintf(stderr, "%s: FIB(5)=%d, all okay\n", __func__, fun(5));

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
