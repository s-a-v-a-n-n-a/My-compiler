#define _CRT_SECURE_NO_WARNINGS

#include "lexical_analyzer.h"

Code* code_new(size_t amount)
{
    Code* new_one = (Code*)calloc(1, sizeof(Code));

    if (!new_one)
        return NULL;

    if (code_construct(new_one, amount))
    {
        free(new_one);
        return NULL;
    }

    return new_one;
}

code_errors code_construct(Code *thus, size_t amount)
{
    thus->array = (Lexem*)calloc(amount, sizeof(Lexem));
    if (!thus->array)
        return CODE_NO_CONSTRUCT;
    thus->capacity = amount;
    thus->size = 0;

    return CODE_OK;
}

code_errors code_delete(Code* thus)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking; 

    code_destruct(thus);
    
    free(thus);
    thus = NULL;

    return CODE_OK;
}

code_errors code_destruct(Code* thus)
{
    if (thus->array)
    {
        free(thus->array);
        thus->array = NULL;
    }

    return CODE_OK;
}

code_errors code_resize(Code* thus)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    size_t new_cap = thus->capacity * 2;

    Lexem* new_pointer = (Lexem*)realloc(thus->array, new_cap * sizeof(Lexem));
    if (!new_pointer)
        return CODE_NO_MEMORY;

    thus->array = new_pointer;
    thus->capacity = new_cap;

    return CODE_OK;
}

code_errors code_check_pointer(Code* thus)
{
    if (!thus || !thus->array)
        return CODE_NULL;

    if ((size_t)(thus) <= 4096 || (size_t)(thus->array) <= 4096)
        return CODE_SEG_FAULT;

    return CODE_OK;
}

code_errors code_insert_number(Code* thus, double value)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    if (thus->size + 1 >= thus->capacity)
        code_resize(thus);

    thus->array[thus->size].type = NUMBER;
    thus->array[thus->size].number = value;

    thus->size++;

    return CODE_OK;
}

code_errors code_insert_comparison(Code* thus, comparison sign)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    if (thus->size + 1 >= thus->capacity)
        code_resize(thus);

    thus->array[thus->size].type = COMPARISON;
    thus->array[thus->size].comp = sign;

    thus->size++;

    return CODE_OK;
}

code_errors code_insert_variable(Code* thus, wchar_t** pointer, size_t len)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    if (thus->size + 1 >= thus->capacity)
        code_resize(thus);

    thus->array[thus->size].type = VARIABLE;
    thus->array[thus->size].variable = *pointer;
    thus->array[thus->size].variable_length = len;

    thus->size++;

    return CODE_OK;
}

code_errors code_insert_key_word(Code* thus, key_word key)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    if (thus->size + 1 >= thus->capacity)
        code_resize(thus);

    thus->array[thus->size].type = KEY_WORD;
    thus->array[thus->size].key = key;

    thus->size++;

    return CODE_OK;
}

code_errors code_insert_operation(Code* thus, operations op)
{
    code_errors checking = code_check_pointer(thus);
    if (checking)
        return checking;

    if (thus->size + 1 >= thus->capacity)
        code_resize(thus);

    thus->array[thus->size].type = OPERATION;
    thus->array[thus->size].operation = op;

    thus->size++;

    return CODE_OK;
}


static wchar_t* code_skip_space(wchar_t* pointer)
{
    if (!pointer)
        return NULL;

    while (isspace(*pointer) || *pointer == L'\n' || *pointer == L'\t')
    {
        pointer++;

        if (*pointer == L'\0')
            break;
    }
    return pointer;
}

static wchar_t* code_skip_letters(wchar_t* pointer)
{
    if (!pointer)
        return NULL;

    while (*pointer >= L'a' && *pointer <= L'z' 
         ||*pointer >= L'A' && *pointer <= L'Z'
         ||*pointer >= L'а' && *pointer <= L'я'
         ||*pointer >= L'А' && *pointer <= L'Я' || *pointer == '_')
    {
        pointer++;

        if (*pointer == L'\0')
            break;
    }
    return pointer;
}

comparison code_get_comparison(wchar_t** pointer)
{
    wchar_t sign = **pointer;
    comparison comp_identifier = COMP_EQUAL;

    switch (sign)
    {
    case L'>':
        comp_identifier = COMP_MORE;
        break;
    case L'<':
        comp_identifier = COMP_LESS;
        break;
    case L'=':
        comp_identifier = COMP_EQUAL;
        break;
    case L'!':
        comp_identifier = COMP_NOT_EQUAL;
        break;
    default:
        break;
    }

    (*pointer)++;

    return comp_identifier;
}

double code_get_number(wchar_t **pointer)
{
    double value = 0;
    swscanf(*pointer, L"%lg", &value);

    char was_point = 0;
    while (isdigit(**pointer) || **pointer == '.' || **pointer == '-')
    {
        if (**pointer == L'.' && !was_point)
            was_point++;
        else if (**pointer == L'.')
            break;
        (*pointer)++;
    }

    return value;
}

operations code_get_operation(wchar_t **pointer)
{
    wchar_t first_operation_lexem = **pointer;
    operations op = NO_OP;

    wchar_t whole_op[MAX_OP_LEN] = {};
    swscanf(*pointer, L"%ls", whole_op);

    switch (first_operation_lexem)
    {
    case L'д':
        if (!wcscmp(whole_op, L"додай"))
        {
            op = OP_PLUS;
            (*pointer) += PLUS_LEN;
        }
        break;
    case L'в':
        if (!wcscmp(whole_op, L"вiдкуси"))
        {
            op = OP_SUB;
            (*pointer) += MINUS_LEN;
        }
        else
            op = NO_OP;
        break;
    case L'т':
        if (!wcscmp(whole_op, L"трошки"))
        {
            op = OP_INC;
            (*pointer) += INC_LEN;
        }
        break;
    case L'и':
        if (!wcscmp(whole_op, L"или"))
        {
            op = OP_OR;
            (*pointer) += OR_LEN;
        }
        else if (!wcscmp(whole_op, L"и"))
        {
            op = OP_AND;
            (*pointer) += AND_LEN;
        }
        break;
    case L'*':
        op = OP_MUL;
        (*pointer) += SIMPLE_LEN;
        break;
    case L'/':
        op = OP_DIV;
        (*pointer) += SIMPLE_LEN;
        break;
    case L's':
        if (!wcscmp(whole_op, L"sin"))
        {
            op = OP_SIN;
            (*pointer) += SIN_LEN;
        }
        else if (!wcscmp(whole_op, L"sqrt"))
        {
            op = OP_SQRT;
            (*pointer) += SQRT_LEN;
        }
        break;
    case L'c':
        if (!wcscmp(whole_op, L"cos"))
        {
            op = OP_COS;
            (*pointer) += COS_LEN;
        }
        break;
    case L'l':
        if (!wcscmp(whole_op, L"ln"))
        {
            op = OP_LN;
            (*pointer) += LN_LEN;
        }
        break;
    default:
        break;
    }

    return op;
}

key_word code_get_key_word(wchar_t** pointer)
{
    wchar_t first_operation_lexem = **pointer;
    key_word word = NO_KEY;

    wchar_t whole_key[MAX_KEY_LEN] = {};
    wchar_t additional_key[MAX_KEY_LEN] = {};

    swscanf(*pointer, L"%ls", whole_key);
    switch (first_operation_lexem)
    {
    case L'х':
        if (!wcscmp(whole_key, L"хата"))
        {
            word = KEY_FUNCTION;
            (*pointer) += FUNC_LEN;
        }
        else if (!wcscmp(whole_key, L"хай"))
        {
            word = KEY_SEPARATOR;
            (*pointer) += SEP_LEN;
        }
        break;
    case L'(':
        word = KEY_OPEN_PRIORITY;
        (*pointer) += PRIORITY_LEN;
        break;
    case L')':
        word = KEY_CLOSE_PRIORITY;
        (*pointer) += PRIORITY_LEN;
        break;
    case L'п':
        if (!wcscmp(whole_key, L"пока"))
        {
            swscanf(*pointer + WHILE_OFFSET_LEN, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"що"))
            {
                word = KEY_WHILE;
                (*pointer) += WHILE_LEN;
            }
        }
        else if (!wcscmp(whole_key, L"побачит"))
        {
            word = KEY_PRINT;
            (*pointer) += PRINT_LEN;
        }
        break;
    case L'я':
        if (!wcscmp(whole_key, L"якщо"))
        {
            word = KEY_IF;
            (*pointer) += IF_LEN;
        }
        else if (!wcscmp(whole_key, L"як"))
        {
            word = KEY_ASSIGN;
            (*pointer) += ASSIGN_LEN;
        }
        break;
    case L'б':
        if (!wcscmp(whole_key, L"буде"))
        {
            word = KEY_DECL;
            (*pointer) += DECL_LEN;
        }
        break;
    case L'м':
        if (!wcscmp(whole_key, L"маэ"))
        {
            word = KEY_LEFT_BRACKET;
            (*pointer) += LEFT_BRACKET_LEN;
        }
        break;
    case L'в':
        if (!wcscmp(whole_key, L"всерединi"))
        {
            word = KEY_END_PARAM;
            (*pointer) += END_PARAM_LEN;
        }
        else if (!wcscmp(whole_key, L"в"))
        {
            swscanf(*pointer + RIGHT_BRACKET_OFFSET, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"хате"))
            {
                word = KEY_RIGHT_BRACKET;
                (*pointer) += RIGHT_BRACKET_LEN;
            }
        }
        else if (!wcscmp(whole_key, L"викличе"))
        {
            word = KEY_CALLING;
            (*pointer) += KEY_CALLING_LEN;
        }
        break;
    case L'т':
        if (!wcscmp(whole_key, L"тiкае"))
        {
            swscanf(*pointer + RETURN_OFFSET_LEN, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"звiдси"))
            {
                word = KEY_RETURN;
                (*pointer) += RETURN_LEN;
            }
        }
        break;
    case L'з':
        word = KEY_PARAM;
        (*pointer) += PARAM_LEN;
        break;
    case L'ц':
        if (!wcscmp(whole_key, L"це"))
        {
            word = KEY_LINE_COMMENT;
            (*pointer) += LINE_COMMENT_LEN;
        }
        break;
    case L'д':
        if (!wcscmp(whole_key, L"даст"))
        {
            swscanf(*pointer + SCAN_OFFSET, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"сюды"))
            {
                word = KEY_SCAN;
                (*pointer) += SCAN_LEN;
            }
        }
        break;
    case L'ш':
        if (!wcscmp(whole_key, L"шо"))
        {
            swscanf(*pointer + BLOCK_COM_BEGIN_FIRST_OFFSET, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"це"))
            {
                swscanf(*pointer + BLOCK_COM_BEGIN_SECOND_OFFSET, L"%ls", additional_key);
                if (!wcscmp(additional_key, L"таке"))
                {
                    word = KEY_BLOCK_COMMENT_BEGIN;
                    (*pointer) += BLOCK_COMMENT_BEGIN_LEN;
                }
            }
        }
        break;
    case L'а':
        if (!wcscmp(whole_key, L"а"))
        {
            swscanf(*pointer + BLOCK_COM_END_FIRST_OFFSET, L"%ls", additional_key);
            if (!wcscmp(additional_key, L"ось"))
            {
                swscanf(*pointer + BLOCK_COM_END_SECOND_OFFSET, L"%ls", additional_key);
                if (!wcscmp(additional_key, L"таке"))
                {
                    word = KEY_BLOCK_COMMENT_END;
                    (*pointer) += BLOCK_COMMENT_END_LEN;
                }
            }
        }
        break;
    default:
        break;
    }

    return word;
}

//int check_variable(wchar_t *var)
//{
//    size_t var_len = wcslen(var);
//    for (size_t i = 0; i < var_len; i++)
//        if (!(iswalnum(var[i]) || var[i] == L'_'))
//            return 0;
//    return 1;
//}

Code* code_general(wchar_t *input_array)
{
    size_t input_array_length = wcslen(input_array);
    wchar_t* current_symbol_pointer = input_array;

    Code* new_code = code_new(10);//поменять на константу потом

    while (*current_symbol_pointer)
    {
        current_symbol_pointer = code_skip_space(current_symbol_pointer);
        //printf("%p\n", current_symbol_pointer);
        if (isdigit(*current_symbol_pointer) || (*current_symbol_pointer) == '-')
        {
            double number = code_get_number(&current_symbol_pointer);
            code_insert_number(new_code, number);
        }
        else if (isspace(*current_symbol_pointer))
        {
            current_symbol_pointer = code_skip_space(current_symbol_pointer);
        }
        else if (*current_symbol_pointer == L'>'
              || *current_symbol_pointer == L'<'
              || *current_symbol_pointer == L'='
              || *current_symbol_pointer == L'!')
        {
            comparison sign = code_get_comparison(&current_symbol_pointer);
            code_insert_comparison(new_code, sign);
        }
        else
        {
            //printf("here\n");
            key_word if_key = NO_KEY;
            wchar_t str[MAX_VAR_NAME] = {};

            operations if_op = code_get_operation(&current_symbol_pointer);
            if (if_op == NO_OP)
            {
                if_key = code_get_key_word(&current_symbol_pointer);
                if (if_key == NO_KEY)
                {
                    //printf("it is not key\n");
                    swscanf(current_symbol_pointer, L"%ls", str);
                    //wprintf(L"%ls\n", str);
                    //int variable_validity = check_variable(str);
                    //if (variable_validity)
                      //  return NULL;
                    code_insert_variable(new_code, &current_symbol_pointer, wcslen(str));
                    current_symbol_pointer = code_skip_letters(current_symbol_pointer);
                }
                else
                {
                    code_insert_key_word(new_code, if_key);
                }
            }
            else
            {
                code_insert_operation(new_code, if_op);
            }
        }
        current_symbol_pointer++;
    }

    return new_code;
}

Code *code_file_reader(const char *name)
{
    FILE* input = text_read(name); //rb?

    Catalog* base = text_init(input);

    Code* new_code = code_general(base->buffer);
    if (!new_code)
        return NULL;

    for (size_t i = 0; i < new_code->size; i++)
        printf("%d ", new_code->array[i].type);
    printf("\n");

    new_code->first_array = base->buffer;
    base->buffer = NULL;
    text_delete(base);
    fclose(input);

    return new_code;   
    /*fgetws(bufer, 1000, input);
    wprintf(L"%ls\n", bufer);

    fclose(input);

    Code* new_one = code_general(bufer);

    for (size_t i = 0; i < new_one->size; i++)
        printf("%d ", new_one->array[i].type);
    printf("\n");

    free(bufer);*/
}