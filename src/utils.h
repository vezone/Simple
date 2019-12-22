#pragma once

#include <stdint.h>

typedef int32_t int32;
typedef int64_t int64;
typedef struct file_info file_info;

struct file_info 
{
    char* filepath;
    char* filename;
    int32 is_saved;
    int32 tab_number;
    file_info* next;
};

file_info* file_info_get(int32 page_number);
void file_info_add(int32 page_number, const char* filepath);
void file_info_remove(int32 tab_number);
void file_info_set_saved(int32 tab_number);
int32 file_info_is_saved(int32 tab_number);
void file_info_free();
void file_info_print();

int32 vstring_length(const char* string);
int32 vstring_compare(const char* left, const char* right);
char* vstring_copy(const char* to_copy);
char* file_read(const char* path);
void  file_write(const char* path, char* data);
char* file_get_name(const char* filepath);
