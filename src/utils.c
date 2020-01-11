#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

static file_info* g_file_data = NULL;

file_info* 
file_info_get(int page_number)
{
    file_info* ptr = g_file_data;
    for ( ; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->tab_number == page_number)
        {
            return ptr;
        }
    }
    return NULL;
}

void
file_info_add(int page_number, const char* filepath)
{
    if (g_file_data == NULL)
    {
        g_file_data = malloc(sizeof(file_info));
        g_file_data->filepath = vstring_copy(filepath);
        g_file_data->filename = vstring_copy(file_get_name(filepath));
        g_file_data->tab_number = page_number;
        g_file_data->is_saved = 0;
        g_file_data->next = NULL;
    }
    else 
    {
        file_info* ptr;
        for (ptr = g_file_data; ptr->next != NULL; ptr = ptr->next);
        ptr->next = malloc(sizeof(file_info));
        ptr->next->filepath = vstring_copy(filepath);
        ptr->next->filename = vstring_copy(file_get_name(filepath));
        ptr->next->tab_number = page_number;
        g_file_data->is_saved = 0;
        ptr->next->next = NULL;
    }
}

//TODO: rework
void
file_info_remove(int32 tab_number)
{
    if (g_file_data != NULL)
    {
        file_info* ptr = g_file_data;
        file_info* temp_prev = NULL;
        file_info* temp_next;
        for ( ; ptr != NULL; ptr = ptr->next)
        {
            if (ptr->tab_number == tab_number)
            {
                if (ptr->filename) { free(ptr->filename); }
                if (ptr->filepath) { free(ptr->filepath); } 
                if (temp_prev != NULL) { temp_prev->next = ptr->next; }
                else { g_file_data = ptr->next; }
                return;
            }
            temp_prev = ptr;
        }
    }
}

void 
file_info_set_saved(int32 tab_number)
{
    file_info* ptr;
    for (ptr = g_file_data; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->tab_number == tab_number)
        {
            ptr->is_saved = 1;
            return;
        }
    }
}

int32 file_info_is_saved(int32 tab_number)
{
    file_info* ptr;
    for (ptr = g_file_data; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->tab_number == tab_number)
        {
            printf("ptr is saved: %d \n", ptr->is_saved);
            return ptr->is_saved;
        }
    }
    return 0;
}

// wip 
// TODO: fix bugs 
void 
file_info_free()
{
    file_info* ptr = g_file_data;
    if (ptr != NULL)
    {
        while (ptr->next != NULL)
        {
            if (ptr->filepath) { /* printf("file path: %s\n", ptr->filepath); */ free(ptr->filepath); }
            if (ptr->filename) { /* printf("file path: %s\n", ptr->filename); */ free(ptr->filename); }
            ptr = ptr->next;
        }
    }
    else
    {
        printf("file info is empty!\n");
    }
}

void file_info_print()
{   
    file_info* ptr = g_file_data;
    printf("PRINT\n");
    if (ptr != NULL)
    {
        for ( ; ptr != NULL; ptr = ptr->next)
        {
            printf("number: %d, file name: %s, file path: %s\n", 
                ptr->tab_number, ptr->filename, ptr->filepath);
        }
    }
}

int32 vstring_length(const char* string)
{
    int32 length;
    for (length = 0; string[length] != '\0'; length++);
    return length;
}

int32 vstring_compare(const char* left, const char* right)
{
    int32 left_length = vstring_length(left);
    int32 right_length = vstring_length(right);

    if ((left_length == 0) || (right_length == 0))
    {
        return 0;
    }
    else if (left_length != right_length)
    {
        return 0;
    }
    else 
    {
        for (right_length = 0; right_length > left_length; right_length++)
        {
            if (left[right_length] != right[right_length])
            {
                return 0;
            }
        }
    }

    return 1;
}

char* vstring_copy(const char* to_copy)
{
    int32 i;
    int32 to_copy_length = vstring_length(to_copy);
    char* copy = malloc((to_copy_length + 1) * sizeof(char));
    for (i = 0; i < to_copy_length; i++)
    {
        copy[i] = to_copy[i];
    }
    copy[to_copy_length] = '\0';
    return copy;
}

char* vstring_repeate(const char* to_repeate, int32 count)
{
    if ((count <= 0) || (vstring_compare(to_repeate, "")))
    {
        return "";
    }
    
    int32 i;
    int32 j;
    int32 id;
    int32 to_repeate_length = vstring_length(to_repeate);
    int32 new_length = count * to_repeate_length;
    char* new_string = malloc((new_length + 1) * sizeof(char));
    for (i = 0, id = 0; i < count; i++)
    {
        for (j = 0; j < to_repeate_length; j++)
        {
            new_string[id] = to_repeate[j];
            ++id;
        }
    }
    new_string[new_length] = '\0';
    return new_string;
}

const char* file_read(const char* path)
{
    long file_length;
    char* file_content;
    FILE* file;
    file = fopen(path, "rb");
    if (file != NULL)
    {
        fseek(file, 0, 2);
        file_length = ftell(file);
        fseek(file, 0, 0);
        file_content = malloc((file_length + 1) * sizeof(char));
        fread(file_content, sizeof(char), file_length, file);
        file_content[file_length] = '\0';
        fclose(file);
    }
    else
    {
        file_content = 0;
    }
    return (const char*)file_content;
}

void file_write(const char* path, const char* data)
{
    FILE* file;
    file = fopen(path, "wb");
    if (file != NULL)
    {
        if (data != NULL)
        {
            int32 data_length = vstring_length(data);
            fwrite(data, sizeof(char), data_length, file);
        }
        fclose(file);
    }
}

char* file_get_name(const char* filepath)
{
    int32 filepath_length = vstring_length(filepath);
    int32 last_address = 0;
    int32 new_length;
    int32 i = 0;
    char* filename;

    for ( ; i < filepath_length; i++)
    {
        char el = filepath[i];

        if (el == '/')
        {
            last_address = i + 1;
        }
    }

    new_length = filepath_length - last_address;
    filename = malloc((new_length + 1) * sizeof(char));
    for (i = last_address; i < filepath_length; i++)
    {
        filename[i - last_address] = filepath[i];
    }
    filename[new_length] = '\0';

    return filename;
}
