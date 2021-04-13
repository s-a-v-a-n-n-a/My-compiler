#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include <wchar.h>
#include <locale.h>

#include "lexical_consts.h"

typedef struct file_text
{
    wchar_t  *buffer;
    //indicator of end of filled part
    size_t length;
    size_t capacity;
} Catalog;


Catalog   *text_init            (FILE *input);
void       text_delete          (Catalog *base);
void       text_construct       (FILE *input, Catalog *base);
void       text_destruct        (Catalog *base);

void       text_resize          (Catalog *base);

void       text_write_in_buffer (Catalog *base, const wchar_t *information, size_t information_length);

size_t     text_get_file_size   (FILE *file);
wchar_t   *text_get             (FILE *file, size_t *length);

FILE      *text_read            (const char *file_name);
