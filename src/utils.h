#pragma once

#include <stdint.h>

#define	BLACK(x) "\x1B[30m"x"\033[0m"
#define	RED(x) "\x1B[31m"x"\033[0m"  
#define	GREEN(x) "\x1B[32m"x"\033[0m"  
#define	YELLOW(x) "\x1B[33m"x"\033[0m"  
#define BLUE(x) "\x1B[34m"x"\033[0m"
#define	MAGNETA(x) "\x1B[35m"x"\033[0m"  
#define	CYAN(x) "\x1B[36m"x"\033[0m"  
#define WHITE(x) "\x1B[37m"x"\033[0m"  

#define	BRIGHTBLACK(x) "\x1B[90m"x"\033[0m"  
#define BRIGHTRED(x) "\x1B[91m"x"\033[0m"  
#define BRIGHTGREEN(x) "\x1B[92m"x"\033[0m"  
#define BRIGHTYELLOW(x) "\x1B[93m"x"\033[0m"  
#define BRIGHTBLUE(x) "\x1B[94m"x"\033[0m"  
#define BRIGHTMAGNETA(x) "\x1B[95m"x"\033[0m"  
#define BRIGHTCYAN(x) "\x1B[96m"x"\033[0m"   
#define BRIGHTWHITE(x) "\x1B[97m"x"\033[0m" 

#define RED5(x) "\033[5;1;31m"x"\033[0m"
#define REDBG5(x) "\033[5;101;30m"x"\033[0m"

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
const char* file_read(const char* path);
void  file_write(const char* path, const char* data);
char* file_get_name(const char* filepath);
