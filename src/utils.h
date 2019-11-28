#pragma once

#include <stdint.h>

typedef int32_t int32;
typedef int64_t int64;
typedef struct FileInfo FileInfo;

struct FileInfo 
{
    char* filepath;
    char* filename;
    int tab_number;
    FileInfo* next;
};

FileInfo* file_info_get(int page_number);
void file_info_add(int page_number, const char* filepath);
void file_info_remove(int32 tab_number);
void file_info_free();

int32 vstring_length(const char* string);
int32 vstring_compare(const char* left, const char* right);
char* vstring_copy(const char* to_copy);
char* file_read(const char* path);
void  file_write(const char* path, char* data);
char* file_get_name(const char* filepath);
