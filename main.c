#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "httpdlopen.h"

int main(void)
{
    void* library;

    httpdlopen_init();

    httpdlopen_set("google", "http://google.com/");
    httpdlopen_set("yahoo", "http://yahoo.com");
    //httpdlopen_set("google", "http://google.ru/");

    library = httpdlopen_get("yahoo");
    printf("%s", (const char*)library);

    httpdlopen_deinit();

    return 0;
}
