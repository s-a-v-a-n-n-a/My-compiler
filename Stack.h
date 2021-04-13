#define _CRT_SECURE_NO_WARNINGS

#pragma once

/**
    \file
    File with functions for using my stack
    It has constructor, destructor, push, pop, back and resize.
    There is also defend from wrong using of it.
    Thank you for using this program!
    \warning This stack works only with doubles \n
    \authors Anna Savchuk
    \note    If any function gets errors not like STACK_OK, they send it to stack_dump \n
             At the end the last information about stack will be added to log_file
    \date    Last update was 10.24.20 at 22:40
    \todo    add codecopy in Stack_Struct -> change verifier
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Stack_struct Structure;
typedef struct Defeat_stack Stack;

#include "generating_asm_code.h"

typedef enum stack_code_errors
{
    STACK_OK,
    STACK_NULL,
    STACK_SEG_FAULT,
    STACK_DELETED,
    STACK_NO_CONSTRUCT,
    STACK_UNDERFLOW,
    STACK_NO_MEMORY,
    STACK_TOO_BIG,
    STACK_DEAD_CANARY,
    STACK_INVADERS,
    STACK_TRANSACTION_ERROR,
    STACK_TRANSACTION_OK
} stack_code;

//сделать extern
extern const char STACK_CONSTRUCT_MESSAGE[];
extern const char STACK_DESTRUCT_MESSAGE[];
extern const char STACK_PUSH_MESSAGE[];
extern const char STACK_RESIZE_MESSAGE[];
extern const char STACK_POP_MESSAGE[];
extern const char STACK_BACK_MESSAGE[];

const double EPSILON = 0.000001;

const size_t MIN_STACK_SIZE = 2;

#define ASSERTION(code)                                                   \
    fprintf(stderr, "-----------------!WARNING!----------------\n");      \
    fprintf(stderr, "IN FILE %s\nIN LINE %d\n", __FILE__, __LINE__);      \
    assertion(code);                                                      \


#define VERIFYING(that_stack, mode);                                      \
    stack_code check = stack_verifier(that_stack);                        \
    if (check != STACK_OK)                                                \
    {                                                                     \
        ASSERTION(check);                                                 \
        stack_dump((*that_stack), check, mode);                           \
    }                                                                     \


#define VERIFYING_DESTRUCT(that_stack);                                   \
    stack_code check = stack_verifier(that_stack);                        \
    if (check != STACK_OK)                                                \
    {                                                                     \
        ASSERTION(check);                                                 \
        stack_dump((*that_stack), check, STACK_DESTRUCT_MESSAGE);                 \
    }                                                                     \
    else                                                                  \
        stack_dump(*that_stack, STACK_OK, STACK_DESTRUCT_MESSAGE);                                                                     \

typedef Frame stack_elem;
//#define elem "%lg"

struct Stack_struct
{
    int         canary_before;
    stack_elem** buffer;
    size_t      length;
    size_t      capacity;
    long int    hash_buffer;
    long int    hash_stack;
};

struct Defeat_stack
{
    int           canary_first;
    Structure* stack;
    Structure* stack_copy;
    int           canary_last;
};

//#define COPYING

#ifdef COPYING
static Stack* cage_copy;
#endif

/*!
Prints the errors in the console
@param[in]        code                The identifier of the error
*/
void              assertion(stack_code code);

/*!
Outputs the information about the current state of the stack into the file
@param[in]       *log                 The file to write in
@param[in]       *that_stack          The pointer on the shell of the stack
*/
void              print_state_stack(FILE* log, Stack* that_stack);

/*!
Outputs the information about the current state of the stack into "log_file.txt"
@param[in]       *that_stack          The pointer on the shell of the stack
@param[in]        code                The code of the mistake
@param[in]        who                 The code of the function requested for dump
*/
void              stack_dump(Stack* that_stack, stack_code code, const char* who);

/*!
Checks if the pointer is valid
@param[in]       *that_stack          The pointer on the shell of the stack
Returns  STACK_NULL                   If the pointer does not exists
         STACK_SEG_FAULT              If the pointer points in prohibited for using block of memory
         STACK_OK                     If the pointer is valid
*/
stack_code        is_pointer_valid(Stack* that_stack);

/*!
Counts hash for the buffer in the stack by algorythm Adler-32
@param[in]       *that_stack          The pointer on the shell of the stack
@param[out]       hash_sum            The hash of the buffer in the stack
*/
static long int   hashing_buffer(Structure* that_stack);

/*!
Counts hash for the whole stack by algorythm Adler-32
@param[in]       *that_stack          The pointer on the shell of the stack
@param[out]       hash_sum            The hash of the whole stack
*/
static long int   hashing_stack(Structure* that_stack);

/*!
Checks if the stack was spoiled and returns it to last saved correct version if it was
@param[in]       *that_stack          The pointer on the shell of the stack
Returns  STACK_OK                     If everything is okay
*/
static stack_code reserve_copy(Structure** that_stack, Structure** copy_stack);

/*!
Frees the spoiled stack and creates new due to the copy
@param[in]      **stack_1             The pointer on pointer on the shell of the spoiled stack
@param[in]       *stack_2             The pointer on the shell of the copy of stack
Returns  STACK_TRANSACTION_OK         If the stack was transacted\n
         STACK_TRANSACTION_ERROR      If it was impossible to transact\n
*/
static stack_code transaction(Structure** stack_1, Structure* stack_2);

/*!
Checks all of the states of the stack
@param[in]      *that_stack           The pointer on the shell of the stack
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_verifier(Stack** that_stack);

/*!
Creates new stack
@param[in]        size                The size of the future buffer of the stack
@param[out]      *stack_tmp           The pointer on the shell of the future stack
*/
Stack* stack_new(size_t size);

/*!
Constructs the stack
@param[in]      **that_stack          The stack
Returns  STACK_OK                     If everything is ok
         STACK_NO_CONSTRUCT           If it was impossible to create the stack
*/
static stack_code stack_construct(Structure** that_stack, size_t stack_size);

/*!
Destructs the shell of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
Returns  STACK_OK                     If everything is ok\n
*/
stack_code        stack_destruct(Stack** that_stack);

/*!
Destructs the stack
@param[in]      **that_stack          The stack
Returns  STACK_OK                     If everything is ok\n
*/
stack_code        stack_destruct_inside(Structure** that_stack);

/*!
Changes the capacity of the stack
@param[in]      **that_stack          The pointer on the shell of the stack
@param[in]       *stack               The stack from the shell to resize
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_NO_MEMORY              If there is no memory to resize\n
         STACK_TOO_BIG                If the needed memory is too big\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code stack_resize(Stack** that_stack, Structure* stack, const double amount);

/*!
Adds value to the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]        value               The value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_push(Stack** that_stack, stack_elem *value);

/*!
Delets value from the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]       *value               The pointer on the value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code        stack_pop(Stack** that_stack, stack_elem** value);

/*!
Shows value from the end of the stack
@param[in]      **that_stack          The pointer on pointer on the shell of the stack
@param[in]       *value               The pointer on the value wanted to be pushed
Returns  STACK_OK                     If everything is ok\n
         STACK_NULL                   If there wasn't pointers on units of stack\n
         STACK_SEG_FAULT              If memory access denied\n
         STACK_DELETED                If some of the units were deleted\n
         STACK_DEAD_CANARY            If the stack was spoiled in bolders\n
         STACK_INVADERS               If the stack was spoiled inside\n
         STACK_TRANSACTION_ERROR      If the stack was spoiled and there were troubles with memory to fix it\n
         STACK_TRANSACTION_OK         If the stack was spoiled and it was fixed\n
*/
stack_code stack_back(Stack** that_stack, stack_elem** value);

stack_code stack_earlier_back(Stack** that_stack, stack_elem** value);

