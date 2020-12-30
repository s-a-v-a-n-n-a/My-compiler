#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include <wchar.h>
#include <locale.h>

#include "lexical_consts.h"
#include "catalog_tree_read_from_file.h"

typedef struct Lexem
{
    char type;
    
    double number;
    wchar_t* variable;
    size_t variable_length;
    operations operation;
    comparison comp;
    key_word key;

    size_t amount;
}Lexem;

typedef struct Current_code
{
    Lexem* array;
    wchar_t* first_array;
    size_t size;
    size_t capacity;
}Code;

typedef enum code_errors
{
    CODE_OK,
    CODE_NULL,
    CODE_SEG_FAULT,
    CODE_DELETED,
    CODE_NO_CONSTRUCT,
    CODE_NO_MEMORY
} code_errors;

Code* code_new(size_t amount);
code_errors code_construct(Code* thus, size_t amount);
code_errors code_delete(Code* thus);
code_errors code_destruct(Code* thus);

code_errors code_resize(Code* thus);

code_errors code_insert_number(Code* thus, double value);
code_errors code_insert_comparison(Code* thus, comparison sign);
code_errors code_insert_variable(Code* thus, wchar_t** pointer, size_t len);
code_errors code_insert_key_word(Code* thus, key_word key);
code_errors code_insert_operation(Code* thus, operations op);

code_errors code_check_pointer(Code* thus);


static wchar_t* code_skip_space(wchar_t* pointer);
static wchar_t* code_skip_letters(wchar_t* pointer);

comparison code_get_comparison(wchar_t** pointer);
double code_get_number(wchar_t** pointer);
operations code_get_operation(wchar_t** pointer);
key_word code_get_key_word(wchar_t** pointer);

Code* code_general(wchar_t* input_array);
Code* code_file_reader(const char* name);