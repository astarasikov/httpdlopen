#ifndef __HTTP_DLOPEN_TABLE_H__
#define __HTTP_DLOPEN_TABLE_H__

void httpdlopen_set(const char* name, const char* url);
void* httpdlopen_get(const char* name);

void httpdlopen_init();
void httpdlopen_deinit();

#endif // __HTTP_DLOPEN_TABLE_H__
