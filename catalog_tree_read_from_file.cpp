#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "catalog_tree_read_from_file.h"


Catalog *text_init(FILE *input)
{
    Catalog *base = (Catalog*)calloc(1, sizeof(Catalog));
    if (!base)
    {
        return NULL;
    }

    text_construct(input, base);
    if (!base->buffer)
    {
        text_destruct(base);

        return NULL;
    }

    return base;
}

void text_delete(Catalog *base)
{
    text_destruct(base);

    free(base);
}

void text_construct(FILE *input, Catalog *base)
{
    base->length   = text_get_file_size(input);
    base->capacity = base->length;
    base->buffer   = text_get(input, &base->capacity);
}

void text_destruct(Catalog *base)
{
    if (base->buffer)
        free(base->buffer);
}

void text_resize(Catalog *base)
{
    size_t new_capacity = base->capacity * 2;

    wchar_t *new_buffer = (wchar_t*)realloc(base->buffer, new_capacity * sizeof(wchar_t));

    assert(new_buffer);

    base->buffer   = new_buffer;
    base->capacity = new_capacity;
}

void text_write_in_buffer(Catalog *base, const wchar_t *information, size_t information_length)
{
    if (base->length + information_length + 2 >= base->capacity)
        text_resize(base);

    //+1 чтоб следующий не скушал последний символ
    wmemcpy(base->buffer + base->length - 1, information, information_length + 1);

    base->length += information_length;
}

size_t text_get_file_size (FILE *file)
{
    assert(file != NULL);

    long int length = 0;

    if (!fseek(file, 0, SEEK_END))
        length = ftell(file);
    length++;

    fseek(file, 0, SEEK_SET);

    return length;
}

wchar_t *text_get(FILE *file, size_t *length)
{
    assert(file != NULL && *length != 0);

    wchar_t *buffer = (wchar_t*) calloc(*length + 1, sizeof(wchar_t));
    if (!buffer)
    {
        return NULL;
    }
    
    //fgetws(buffer, *length + 1, file);
    wchar_t* additional_buffer = (wchar_t*)calloc(*length + 1, sizeof(wchar_t));
    while (fgetws(additional_buffer, *length + 1, file))
    {
        buffer = wcscat(buffer, additional_buffer);
    }
    //assert((size_t)obj == *length - 1);
    //printf("%ld\n", wcslen(buffer));
    //wprintf(L"%ls\n", buffer);


    buffer[*length] = '\0';

    return buffer;
}

FILE *text_read(const char *file_name)
{
    FILE* input = fopen(file_name, "r");
    if (!input)
        return NULL;

    return input;
}
