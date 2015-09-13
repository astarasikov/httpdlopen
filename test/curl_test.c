#include <stdio.h>
#include <string.h>

#include "httpdlopen.h"

int test(const char* name)
{
    void* raw = httpdlopen_get(name);
    const char* lib = (const char*)raw;
    if (lib == NULL)
    {
        printf("%s is NULL\n", name);
        return -1;
    }
    else
    {
        printf("%s len is %lu and data is '%s'\n",
               name, strlen(lib), lib);
        return (int)strlen(lib);
    }
}

int main(void)
{
    int google, yahoo, yandex, mail;

    httpdlopen_init();

    httpdlopen_set("google", "http://google.com/");
    httpdlopen_set("yahoo", "http://yahoo.com");
    httpdlopen_set("yandex", "http://yandex.ru/");

    google = test("google");
    yahoo  = test("yahoo");
    yandex = test("yandex");
    mail   = test("mail");

    httpdlopen_deinit();

    if (google < 100)
        return 1;
    if (yahoo < 100)
        return 1;
    if (yandex != 0)
        return 1;
    if (mail != -1)
        return 1;
    return 0;
}
