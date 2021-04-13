#pragma once

typedef struct Current_frame Frame;

#include "code_tree_builder.h"
#include "Stack.h"

const size_t MIN_FRAME_SIZE = 2;

const char FUNCTION_FRAME = 0;
const char CYCLE_FRAME = 1;
const char CONDITION_FRAME = 2;


extern const char JUMP_LABEL[];

typedef struct Current_frame
{
    char frame_type;

    Var* vars;
    size_t vars_amount;
    size_t vars_capacity;

    wchar_t* current_pointer;
}Frame;

typedef enum Frame_errors
{
    FRAME_OK,
    FRAME_NULL,
    FRAME_NO_MEMORY,
    FRAME_NO_CONSTRUCTION
}frame_code;

Frame*      frame_new                   (size_t amount, char type);
frame_code  frame_construct             (Frame* thus, size_t amount, char type);
frame_code  frame_delete                (Frame* thus);
frame_code  frame_destruct              (Frame* thus);
frame_code  frame_resize                (Frame* thus);
frame_code  frame_insert                (Frame* thus, wchar_t* var, size_t length, size_t args, size_t address);

typedef struct Compiler_state
{
    Diff_tree* code_tree;
    long long index;

    FILE* asm_code;

    Stack *scopes;

    long long number_of_conditional_jump;

    long long unused_values_amount;
} Compiler;

Compiler*   compiler_new                (Diff_tree* tree);
void        compiler_delete             (Compiler* thus);


int         strings_comparison          (wchar_t* var_1, size_t length_1, wchar_t* var_2, size_t length_2);
long long   find_this_var               (Frame* frame, wchar_t* var, size_t length);

void        print_var                   (Diff_tree* tree, long long index);
void        print_params                (Diff_tree* tree, long long index);

void        insert_params               (Frame* frames, Diff_tree* tree, long long index, long long* ram);

long long   control_block_linkers       (Compiler* cmp);
void        make_new_frame              (Diff_tree* tree, long long root, Frame* cur_frame, long long curr_index, long long ram);

Var*        get_variable_structure      (Diff_tree* tree, long long index, Stack* cur_frame);

void        print_variable              (Compiler* cmp, long long address);
int         write_in_variable           (Compiler* cmp, long long index);
int         read_from_variable          (Compiler* cmp, long long index);

void        print_comparison            (Compiler* cmp, comparison CMP);
void        print_operation             (Compiler* cmp, operations OP);

void        print_or                    (Compiler* cmp);
void        print_and                   (Compiler* cmp);

long long   print_jump_for_if           (Compiler* cmp, long long index);
long long   print_jump_for_while        (Compiler* cmp, long long index);

void        write_all_params            (Compiler* cmp, long long index, Var* func);
int         compile_expression          (Compiler *cmp, long long index);

void        reduce_unused_values_amount (Compiler* cmp);
long long   current_line_code           (Compiler *cmp);

void        print_russian_name          (FILE* file, wchar_t* pointer, size_t length);
void        print_const_russian_name    (FILE* file, const wchar_t* pointer);

int         code_generation             (Diff_tree* tree);


