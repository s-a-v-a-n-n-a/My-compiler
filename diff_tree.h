#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <wchar.h>
#include <locale.h>

//#include "Stack.h"
#include "lexical_consts.h"

//#include "catalog_tree_consts.h"
//#include "catalog_tree_read_from_file.h"

const int    ABSENT = -1;

const double RESIZE_UP = 2;
const double RESIZE_DOWN = 0.5;

const int RIGHT = 0;
const int LEFT = 1;

typedef struct Var_struct
{
    wchar_t* variable;
    size_t   variable_length;

    size_t address;

    size_t args_amount;
} Var;

typedef struct Var_param
{
    Var* params;

    size_t size;
    size_t capacity;
} Param;

Param* param_new(size_t amount);
void param_construct(Param* thus, size_t amount);
void param_delete(Param* thus);
void param_destruct(Param* thus);
void param_resize(Param* thus);
void param_insert(Param* thus, wchar_t* var, size_t length);

typedef enum tree_code_errors
{
    TREE_OK,
    TREE_NULL,
    TREE_SEG_FAULT,
    TREE_DELETED,
    TREE_NO_CONSTRUCT,
    TREE_UNDERFLOW,
    TREE_OVERFLOW,
    TREE_NO_MEMORY,
    TREE_TOO_BIG,
    TREE_CONNECT_ERROR
} tree_code;

/*
typedef enum tree_operations_names
{
    OP_PLUS,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_SIN,
    OP_COS,
    OP_LN,
    OP_SQRT,
    OP_DIFF,
    NO_OP
}tree_operations;
*/
extern const int tree_priorities[10];

extern const char* tree_operations_texts[];

typedef struct Diff_tree_node Node;
struct Diff_tree_node
{
    long long parent;
    long long left_son;
    long long right_son;

    int type;

    double number;

    Param* func_params;
    
    wchar_t* variable;
    size_t variable_length;

    operations operation;
    comparison comp;
    key_word key;

    long long depth;

    long long next;
};

typedef struct Diff_tree_type Diff_tree;
struct Diff_tree_type
{
    Node* tree;

    long long size;
    long long capacity;

    long long root_index;
    long long first_free;
};

extern const char* tree_state_text[];

typedef enum tree_functions_identifiers
{
    CONSTRUCTION,
    DESTRUCTION,
    INSERTION,
    RESIZING,
    REMOVING
} tree_functions;

extern const char* TREE_FUNCTION_IDENTIFIERS[];

Diff_tree*       tree_init                          (const long long amount);
void             tree_delete                        (Diff_tree* dtree);
tree_code        tree_construct                     (Diff_tree* dtree, const long long amount);
void             tree_destruct                      (Diff_tree* dtree);

void             tree_copy_node                     (Diff_tree* copy_from, Diff_tree* copy_to, long long root_to_copy, long long* where_to_copy);
void             tree_copy_branch                   (Diff_tree* copy_from, Diff_tree* copy_to, long long root_to_copy, long long where_to_copy);

long long        tree_parent                        (Diff_tree* dtree, long long index);
long long        tree_right_son                     (Diff_tree* dtree, long long index);
long long        tree_left_son                      (Diff_tree* dtree, long long index);

long long        tree_go_up                         (Diff_tree* dtree, long long index, long long steps_amount);

char             tree_get_type                      (Diff_tree* dtree, long long index);
operations       tree_get_operation                 (Diff_tree* dtree, long long index);
key_word         tree_get_key                       (Diff_tree* dtree, long long index);
comparison       tree_get_comp                      (Diff_tree* dtree, long long index);
double           tree_get_number                    (Diff_tree* dtree, long long index);
wchar_t*         tree_get_variable                  (Diff_tree* dtree, long long index);
size_t           tree_get_variable_length           (Diff_tree* dtree, long long index);

long long        tree_get_depth                     (Diff_tree* dtree, long long index);

int              tree_if_lief                       (Diff_tree* dtree, long long index);

tree_code        tree_resize                        (Diff_tree* dtree, const double coefficient);

tree_code        tree_check_index                   (Diff_tree* dtree, long long index);
 
tree_code        tree_change_operation              (Diff_tree* dtree, long long index, operations operation);

tree_code        tree_insert                        (Diff_tree *dtree, long long *index_after);

tree_code        tree_insert_params                 (Diff_tree* dtree, long long* index_after);
tree_code        tree_insert_number                 (Diff_tree *dtree, long long *index_after, double number);   
tree_code        tree_insert_variable               (Diff_tree* dtree, long long* index_after, wchar_t* variable_pointer, size_t variable_length);
tree_code        tree_insert_function               (Diff_tree* dtree, long long* index_after, wchar_t* variable_pointer, size_t variable_length);
tree_code        tree_insert_operation              (Diff_tree* dtree, long long* index_after, operations operation);
tree_code        tree_insert_comparison             (Diff_tree* dtree, long long* index_after, comparison comp);
tree_code        tree_insert_key_word               (Diff_tree* dtree, long long* index_after, key_word key);
tree_code        tree_insert_linker                 (Diff_tree* dtree, long long* index_after);

static void      tree_remove_lief                   (Diff_tree* dtree, long long index_after, int answer);
static void      tree_remove_branch                 (Diff_tree* dtree, long long branch_base);
tree_code        tree_clean_branch                  (Diff_tree* dtree, long long branch_base);
tree_code        tree_remove_knot                   (Diff_tree* dtree, long long knot, long long next_knot);

tree_code        tree_insert_between_comparison     (Diff_tree* dtree, long long* index_above, comparison comp);
tree_code        tree_insert_between_operations     (Diff_tree* dtree, long long* index_above, operations op);

static tree_code tree_check_pointer                 (Diff_tree* dtree);
static void      tree_check_connections             (Diff_tree* dtree, long long index, tree_code* error);

extern const char* tree_operations[];
extern const char* tree_comparisons[];
extern const char* tree_key_words[];

void             tree_print_node                    (Diff_tree* dtree, long long index, FILE* picture);
void             tree_print_errors                  (tree_code code);
void             tree_dump                          (Diff_tree* dtree, tree_code code, const tree_functions function);
tree_code        tree_verifier                      (Diff_tree* dtree, const tree_functions requester);

void             tree_dot_call                      (const char* name_file, const char* expansion);
void             tree_print_message_in_file         (Diff_tree* dtree, long long index, FILE* picture);
void             tree_print_picture_nodes           (Diff_tree* dtree, long long index, FILE* picture);
void             tree_generate_picture_nodes        (Diff_tree* dtree, long long index, FILE* picture);
void             tree_print_picture_sequence        (Diff_tree* dtree, long long index, FILE* picture);
void             tree_print_picture                 (Diff_tree* dtree, const char* picture_name);
