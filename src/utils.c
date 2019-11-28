#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

static FileInfo* g_file_data = NULL;

FileInfo* 
file_info_get(int page_number)
{
    FileInfo* ptr = g_file_data;
    for (; ptr != NULL ;)
    {
        if (ptr->tab_number == page_number)
        {
            return ptr;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    return NULL;
}

void
file_info_add(int page_number, const char* filepath)
{
    if (g_file_data == NULL)
    {
        g_file_data = malloc(sizeof(FileInfo));
        g_file_data->filepath = vstring_copy(filepath);
        g_file_data->filename = vstring_copy(file_get_name(filepath));
        g_file_data->tab_number = page_number;
        g_file_data->next = NULL;
    }
    else 
    {
        FileInfo* ptr = g_file_data;
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        ptr->next = malloc(sizeof(FileInfo));
        ptr->next->filepath = vstring_copy(filepath);
        ptr->next->filename = vstring_copy(file_get_name(filepath));
        ptr->next->tab_number = page_number;
        ptr->next->next = NULL;
    }
}

//work in progress
void
file_info_remove(int32 tab_number)
{
    if (g_file_data != NULL)
    {
        FileInfo* ptr = g_file_data;
        while (ptr->next != NULL)
        {
            if (ptr->tab_number == tab_number)
            {
                FileInfo* temp = ptr->next;
                if (ptr) { free(ptr); }
                ptr = temp;
            }
            else if (ptr->next != NULL && ptr->next->tab_number == tab_number)
            {

            }
            ptr = ptr->next;
        }
    }
}

// wip 
// TODO: fix bugs 
void 
file_info_free()
{
    FileInfo* ptr = g_file_data;
    if (ptr != NULL)
    {
        while (ptr->next != NULL)
        {
            if (ptr->filepath) { free(ptr->filepath); printf("file path: %s\n", ptr->filepath); }
            if (ptr->filename) { free(ptr->filename); printf("file path: %s\n", ptr->filename); }
            ptr = ptr->next;
        }
    }
    else
    {
        printf("file info is empty!\n");
    }
}

char* file_read(const char* path)
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
    return file_content;
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
    char* copy = malloc(to_copy_length * sizeof(char));
    for (i = 0; i < to_copy_length; i++)
    {
        copy[i] = to_copy[i];
    }
    return copy;
}

void file_write(const char* path, char* data)
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

    for (int32 i = 0; i < filepath_length; i++)
    {
        char el = filepath[i];

        if (el == '/')
        {
            last_address = i + 1;
        }
    }

    int32 new_length = filepath_length - last_address;
    char* filename = malloc((new_length + 1) * sizeof(char));
    for (int32 i = last_address; i < filepath_length; i++)
    {
        filename[i - last_address] = filepath[i];
    }
    filename[new_length] = '\0';

    return filename;
}
