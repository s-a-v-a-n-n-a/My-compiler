#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "diff_tree.h"

const int tree_priorities[10] = 
{ 
    5, //+
    4, //-
    3, //*
    3, //div
    2, //pow
    2, //sin
    2, //cos
    2, //log
    2, //sqrt
    1 
};


const char* tree_operations_texts[] =
{
    "+",
    "-",
    "*",
    "/",
    "^",
    "sin",
    "cos",
    "log",
    "sqrt",
    "'"
};


const char* tree_state_text[] =
{
    "EVERYTHING IS OKAY\n",
    "TREE DOES NOT EXIST\n",
    "MEMORY ACCESS DENIED\n",
    "DEALING WITH NON-EXISTENT UNIT OR THE UNIT WAS DAMAGED\n",
    "NO MEMORY FOR CONSTRUCTION\n",
    "NOTHING TO DELETE\n",
    "WRONG ADDRESS\n",
    "MEMORY ACCESS DENIED\n",
    "TOO BIG CAPACITY IS REQUIRED\n",
    "ERROR WITH ACCESS TO ELEMENTS\n"
};

const char* TREE_FUNCTION_IDENTIFIERS[] =
{
    "CONSTRUCTION",
    "DESTRUCTION",
    "INSERTION",
    "RESIZING",
    "REMOVING",
};

#define ASSERTION(code)                                                  \
    fprintf(stderr, "-----------------!WARNING!----------------\n");     \
    fprintf(stderr, "IN FILE %s\nIN LINE %d\n", __FILE__, __LINE__);     \
    tree_print_errors(code);

#define DEBUG


Diff_tree* tree_init(const long long amount)
{
    Diff_tree* dtree = (Diff_tree*)calloc(1, sizeof(Diff_tree));
    if (!dtree)
    {
        ASSERTION(TREE_NO_MEMORY);
        tree_dump(dtree, TREE_NO_MEMORY, CONSTRUCTION);
        return NULL;
    }

    if (tree_construct(dtree, amount) != TREE_OK)
    {
        tree_destruct(dtree);
        return NULL;
    }

    return dtree;
}

void tree_delete(Diff_tree* dtree)
{
    tree_destruct(dtree);

    if (dtree)
        free(dtree);
}

tree_code tree_construct(Diff_tree* dtree, const long long amount)
{
    dtree->tree = (Node*)calloc(amount, sizeof(Node));
    if (!dtree->tree)
    {
        ASSERTION(TREE_NO_MEMORY);
        tree_dump(dtree, TREE_NO_MEMORY, CONSTRUCTION);
        return TREE_NO_MEMORY;
    }
    dtree->capacity = amount;
    dtree->size = 0;

    dtree->tree[0].parent    = ABSENT;
    dtree->tree[0].right_son = ABSENT;
    dtree->tree[0].left_son  = ABSENT;

    dtree->tree[0].type = NUMBER;

    dtree->tree[0].number    = NAN;
    dtree->tree[0].variable  = 0;
    dtree->tree[0].operation = NO_OP;

    dtree->root_index = 0;
    dtree->first_free = 0;

    for (long long i = 0; i < amount; i++)
    {
        dtree->tree[i].next = i + 1;
    }

    return TREE_OK;
}

void tree_destruct(Diff_tree* dtree)
{
    param_delete(dtree->tree->func_params);
    
    if (dtree->tree != NULL);
        free(dtree->tree);
}

void tree_copy_node(Diff_tree* copy_from, Diff_tree* copy_to, long long root_to_copy, long long *where_to_copy)
{
    switch (copy_from->tree[root_to_copy].type)
    {
        case NUMBER:
            tree_insert_number(copy_to, where_to_copy, copy_from->tree[root_to_copy].number);
            break;
        case VARIABLE:
            tree_insert_variable(copy_to, where_to_copy, copy_from->tree[root_to_copy].variable, copy_from->tree[root_to_copy].variable_length);
            break;
        case OPERATION:
            tree_insert_operation(copy_to, where_to_copy, copy_from->tree[root_to_copy].operation);
            break;
        default:
            break;
    }
}

void tree_copy_branch(Diff_tree *copy_from, Diff_tree *copy_to, long long root_to_copy, long long where_to_copy)
{
    if (tree_if_lief(copy_from, root_to_copy))
    {
        tree_copy_node(copy_from, copy_to, root_to_copy, &where_to_copy);
        return;
    }

    long long right_son = tree_right_son(copy_from, root_to_copy);
    long long left_son  = tree_left_son(copy_from, root_to_copy);

    tree_copy_node(copy_from, copy_to, root_to_copy, &where_to_copy);

    if (left_son != ABSENT)
    {
        tree_copy_branch(copy_from, copy_to, left_son, where_to_copy);
    }

    if (right_son != ABSENT)
    {
        tree_copy_branch(copy_from, copy_to, right_son, where_to_copy);
    }
    /*Stack* went_nodes = stack_new(copy_from->size);

    stack_push(&went_nodes, ABSENT);
    stack_push(&went_nodes, ABSENT);
    stack_push(&went_nodes, ABSENT);

    long long root = root_to_copy;

    int counter_in_root = 0;
    int went_left = 1;
    int went_right = 2;

    tree_copy_node(copy_from, copy_to, root_to_copy, &where_to_copy);
    printf("%lld\n", root_to_copy);
    system("pause");
    if (!tree_if_lief(copy_from, root_to_copy))
    {
        while (counter_in_root <= went_right)
        {
            printf("counter %d\n", counter_in_root);
            if (copy_from->tree[root_to_copy].type == NUMBER)
            {
                printf("%lg\n", copy_from->tree[root_to_copy].number);
            }
            else if (copy_from->tree[root_to_copy].type == VARIABLE)
                printf("%c\n", copy_from->tree[root_to_copy].variable);
            else
                printf("%s\n", tree_operations_texts[(int)(copy_from->tree[root_to_copy].operation)]);
            system("pause");
            if (root_to_copy == root)
            {
                counter_in_root++;
                if (counter_in_root == went_left)
                {
                    root_to_copy = tree_left_son(copy_from, root_to_copy);
                }
                else if (counter_in_root == went_right)
                {
                    root_to_copy = tree_right_son(copy_from, root_to_copy);
                }
                else
                    break;
                printf("%lld\n", root_to_copy);
            }

            tree_copy_node(copy_from, copy_to, root_to_copy, &where_to_copy);

            double last_index = 0;
            double index_before_last = 0;
            stack_back(&went_nodes, &last_index);
            stack_earlier_back(&went_nodes, &index_before_last);

            long long right_son = tree_right_son(copy_from, root_to_copy);
            long long left_son = tree_left_son(copy_from, root_to_copy);

            if (left_son != ABSENT && last_index != left_son && index_before_last != left_son)
            {
                root_to_copy = left_son;
                //stack_push(&went_nodes, root_to_copy);
                stack_dump(went_nodes, STACK_OK, STACK_PUSH_MESSAGE);
            }
            else if (right_son != ABSENT && last_index != right_son) //если правого сына и уложат в стек, то после правого точно
            {
                root_to_copy = right_son;
                //stack_push(&went_nodes, root_to_copy);
                stack_dump(went_nodes, STACK_OK, STACK_PUSH_MESSAGE);
            }
            else
            {
                if (tree_if_lief(copy_from, root_to_copy))
                {
                    stack_push(&went_nodes, ABSENT);
                    stack_push(&went_nodes, ABSENT);
                }
                
                stack_pop(&went_nodes, &last_index);
                stack_pop(&went_nodes, &last_index);

                if (root_to_copy == root)
                    break;
                stack_push(&went_nodes, root_to_copy);

                stack_dump(went_nodes, STACK_OK, STACK_POP_MESSAGE);

                root_to_copy = tree_parent(copy_from, root_to_copy);

                where_to_copy = tree_parent(copy_to, where_to_copy);
            }
            printf("%lld\n", root_to_copy);
        }
    }
    printf("HEREHEREHEREHEREHEREHEREHEREHERE\n");
    stack_destruct(&went_nodes);*/
}

long long tree_parent(Diff_tree* dtree, long long index)
{
    if (index != ABSENT)
        return dtree->tree[index].parent;
    else
        return index;
}

long long tree_right_son(Diff_tree* dtree, long long index)
{
    if (index != ABSENT)
        return dtree->tree[index].right_son;
    else
        return index;
}

long long tree_left_son(Diff_tree* dtree, long long index)
{
    if (index != ABSENT)
        return dtree->tree[index].left_son;
    else
        return index;
}

long long tree_go_up(Diff_tree* dtree, long long index, long long steps_amount)
{
    for (long long i = 0; i < steps_amount; i++)
        index = tree_parent(dtree, index);

    return index;
}

char tree_get_type(Diff_tree* dtree, long long index)
{
    return dtree->tree[index].type;
}

operations tree_get_operation(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return NO_OP;
    
    if (tree_get_type(dtree, index) != OPERATION)
        return NO_OP;

    return dtree->tree[index].operation;
}

key_word tree_get_key(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return NO_KEY;

    if (tree_get_type(dtree, index) != KEY_WORD)
        return NO_KEY;

    return dtree->tree[index].key;
}

comparison tree_get_comp(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return NO_COMP;

    if (tree_get_type(dtree, index) != COMPARISON)
        return NO_COMP;

    return dtree->tree[index].comp;
}

double tree_get_number(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return NAN;
    
    if (tree_get_type(dtree, index) != NUMBER)
        return NAN;

    return dtree->tree[index].number;
}

wchar_t* tree_get_variable(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return 0;
    
    if (tree_get_type(dtree, index) != VARIABLE && tree_get_type(dtree, index) != FUNC)
        return 0;

    return dtree->tree[index].variable;
}

size_t tree_get_variable_length(Diff_tree* dtree, long long index)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return 0;

    if (tree_get_type(dtree, index) != VARIABLE && tree_get_type(dtree, index) != FUNC)
        return 0;

    return dtree->tree[index].variable_length;
}

int tree_if_lief(Diff_tree* dtree, long long index)
{
    if (index <= ABSENT)
        return 0;

    if (tree_right_son(dtree, index) == ABSENT && tree_left_son(dtree, index) == ABSENT)
        return 1;
    else
        return 0;
}

tree_code tree_resize(Diff_tree* dtree, const double coefficient)
{
    size_t new_capacity = dtree->capacity * coefficient;

    Node* new_tree = (Node*)realloc(dtree->tree, sizeof(Node) * new_capacity);
    if (!new_tree)
    {
        ASSERTION(TREE_NO_MEMORY);
        tree_dump(dtree, TREE_NO_MEMORY, RESIZING);
        return TREE_NO_MEMORY;
    }

    dtree->tree = new_tree;

    if (coefficient > 1)
    {
        for (long long i = dtree->capacity; i < new_capacity; i++)
        {
            dtree->tree[i].next = i + 1;
        }
    }

    dtree->capacity = new_capacity;

    return TREE_OK;
}

/*long long tree_search(Diff_tree* dtree, const char* message) //??????????????????????????????????????????????????????????
{
    size_t mes_len = strlen(message);

    for (long long i = 0; i < dtree->size; i++)
    {
        long long length = tree_message_length(dtree, i);
        if (mes_len < length)
            continue;

        char* tree_message = tree_message_beginnig(dtree, i);
        for (size_t j = 0; j <= length; j++)
        {
            if (*(tree_message + j) != message[j])
                break;

            if (j == length)
                return i;
        }
    }

    return ABSENT;
}*/

tree_code tree_change_operation(Diff_tree* dtree, long long index, operations operation)
{
    tree_code checking = tree_check_index(dtree, index);
    if (checking != TREE_OK)
        return checking;

    dtree->tree[index].operation = operation;

    return TREE_OK;
}

tree_code tree_check_index(Diff_tree* dtree, long long index)
{
    /*if (index > dtree->size + 1)
    {
        ASSERTION(TREE_OVERFLOW);
        tree_dump(dtree, TREE_OVERFLOW, INSERTION);
        return TREE_OVERFLOW;
    }*/

    if (dtree->size + 1 >= dtree->capacity)
    {
        tree_code error = tree_resize(dtree, RESIZE_UP);
        if (error != TREE_OK)
            return error;
    }

    return TREE_OK;
}

tree_code tree_insert(Diff_tree* dtree, long long* index_after)
{
    tree_code checking = tree_check_index(dtree, *index_after);
    if (checking != TREE_OK)
        return checking;
    
    long long new_node_index = dtree->first_free;
    
    if (*index_after == ABSENT)
    {
        dtree->tree[dtree->first_free].parent = ABSENT;
        dtree->root_index = dtree->first_free;
    }
    else
    {
        //long long parent = tree_parent(dtree, *index_after);
        long long parent_left_son = tree_left_son(dtree, *index_after);

        dtree->tree[new_node_index].parent = *index_after;

        if (parent_left_son == ABSENT)
            dtree->tree[*index_after].left_son = new_node_index;
        else
            dtree->tree[*index_after].right_son = new_node_index;
    }
    dtree->tree[new_node_index].right_son = ABSENT;
    dtree->tree[new_node_index].left_son = ABSENT;

    dtree->tree[new_node_index].number = NAN;
    dtree->tree[new_node_index].variable = 0;
    dtree->tree[new_node_index].variable_length = 0;
    dtree->tree[new_node_index].operation = NO_OP;
    dtree->tree[new_node_index].comp = NO_COMP;
    dtree->tree[new_node_index].key = NO_KEY;

    *index_after = dtree->first_free;
    dtree->first_free = dtree->tree[dtree->first_free].next;

    dtree->size++;

    return TREE_OK;
}

tree_code tree_insert_params(Diff_tree* dtree, long long* index_after)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);
    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = PARAM;

    dtree->tree[new_node_index].func_params = param_new(MIN_PARAM);

    return TREE_OK;
}

tree_code tree_insert_number(Diff_tree* dtree, long long* index_after, double number)
{
    long long new_node_index = dtree->first_free;
    
    tree_code insert_code = tree_insert(dtree, index_after);
    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = NUMBER;

    dtree->tree[new_node_index].number = number;

    return TREE_OK;
}



tree_code tree_insert_variable(Diff_tree* dtree, long long* index_after, wchar_t* variable_pointer, size_t variable_length)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);
    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = VARIABLE;

    dtree->tree[new_node_index].variable = variable_pointer;
    dtree->tree[new_node_index].variable_length = variable_length;

    return TREE_OK;
}

tree_code tree_insert_function(Diff_tree* dtree, long long* index_after, wchar_t* variable_pointer, size_t variable_length)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);
    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = FUNC;

    dtree->tree[new_node_index].variable = variable_pointer;
    dtree->tree[new_node_index].variable_length = variable_length;

    return TREE_OK;
}

tree_code tree_insert_operation(Diff_tree* dtree, long long* index_after, operations operation)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);
    
    if (insert_code != TREE_OK)
        return insert_code;
    
    dtree->tree[new_node_index].type = OPERATION;

    dtree->tree[new_node_index].operation = operation;

    return TREE_OK;
}

tree_code tree_insert_comparison(Diff_tree* dtree, long long* index_after, comparison comp)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);

    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = COMPARISON;

    dtree->tree[new_node_index].comp = comp;

    return TREE_OK;
}

tree_code tree_insert_key_word(Diff_tree* dtree, long long* index_after, key_word key)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);

    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = KEY_WORD;

    dtree->tree[new_node_index].key = key;

    return TREE_OK;
}

tree_code tree_insert_linker(Diff_tree* dtree, long long* index_after)
{
    long long new_node_index = dtree->first_free;

    tree_code insert_code = tree_insert(dtree, index_after);

    if (insert_code != TREE_OK)
        return insert_code;

    dtree->tree[new_node_index].type = LINKER;

    return TREE_OK;
}

static void tree_remove_lief(Diff_tree* dtree, long long index_after, int answer)
{
    if (index_after == ABSENT)
    {
        dtree->tree[dtree->root_index].next = dtree->first_free;
        dtree->first_free = dtree->root_index;
    }
    else
    {
        long long right = tree_right_son(dtree, index_after);
        long long left = tree_left_son(dtree, index_after);

        if (answer == RIGHT)
        {
            dtree->tree[index_after].right_son = ABSENT;
            dtree->tree[right].next = dtree->first_free;
            dtree->first_free = right;
        }
        else
        {
            dtree->tree[index_after].left_son = ABSENT;
            dtree->tree[left].next = dtree->first_free;
            dtree->first_free = left;
        }
    }

    dtree->size--;
}

static void tree_remove_branch(Diff_tree* dtree, long long branch_base)
{
    long long parent = tree_parent(dtree, branch_base);
    long long right = tree_right_son(dtree, branch_base);
    long long left = tree_left_son(dtree, branch_base);
    if (tree_if_lief(dtree, branch_base) && parent >= 0)
    {
        if (tree_right_son(dtree, parent) == branch_base)
        {
            tree_remove_lief(dtree, parent, RIGHT);
            return;
        }
        else
        {
            tree_remove_lief(dtree, parent, LEFT);
            return;
        }
    }

    if (right != ABSENT)
        tree_remove_branch(dtree, right);
    if (left != ABSENT)
        tree_remove_branch(dtree, left);

    if (parent == ABSENT)
    {
        tree_remove_lief(dtree, ABSENT, RIGHT);
        return;
    }
    else
    {
        if (dtree->tree[parent].right_son == branch_base)
        {
            tree_remove_lief(dtree, parent, RIGHT);
            return;
        }
        else
        {
            tree_remove_lief(dtree, parent, LEFT);
            return;
        }
    }
}

tree_code tree_clean_branch(Diff_tree* dtree, long long branch_base)
{
    if (branch_base < ABSENT)
    {
        ASSERTION(TREE_UNDERFLOW);
        tree_dump(dtree, TREE_UNDERFLOW, REMOVING);
        return TREE_UNDERFLOW;
    }
    tree_code error = TREE_OK;

    tree_remove_branch(dtree, branch_base);

    if (dtree->size < dtree->capacity / 4)
    {
        tree_code resize_error = tree_resize(dtree, RESIZE_DOWN);
        if (resize_error != TREE_OK)
            return resize_error;
    }

    return error;
}

tree_code tree_remove_knot(Diff_tree *dtree, long long knot, long long next_knot)
{
    if (knot == ABSENT)
    {
        ASSERTION(TREE_UNDERFLOW);
        tree_dump(dtree, TREE_UNDERFLOW, REMOVING);
        return TREE_UNDERFLOW;
    }

    long long parent = tree_parent(dtree, knot);

    if (next_knot == tree_left_son(dtree, knot))
    {
       tree_clean_branch(dtree, tree_right_son(dtree, knot));
    }
    else
    {
        tree_clean_branch(dtree, tree_left_son(dtree, knot));
    }
    dtree->tree[next_knot].parent = parent;
    if (parent == ABSENT)
    {
        dtree->root_index = next_knot;
    }
    else
    {
        if (knot == tree_left_son(dtree, parent))
            dtree->tree[parent].left_son = next_knot;
        else
            dtree->tree[parent].right_son = next_knot;
    }

    dtree->tree[knot].next = dtree->first_free;
    dtree->first_free = knot;

    return TREE_OK;
}

tree_code tree_insert_between_comparison(Diff_tree* dtree, long long* index_above, comparison comp)
{
    if (*index_above == ABSENT)
    {
        ASSERTION(TREE_UNDERFLOW);
        tree_dump(dtree, TREE_UNDERFLOW, REMOVING);
        return TREE_UNDERFLOW;
    }

    long long parent = tree_parent(dtree, *index_above);
    char was_left = (*index_above == tree_left_son(dtree, parent));
    if (was_left)
        dtree->tree[parent].left_son = ABSENT;
    else
        dtree->tree[parent].right_son = ABSENT;

    tree_insert_comparison(dtree, &parent, comp);
    dtree->tree[parent].left_son = *index_above;
    *index_above = parent;

    return TREE_OK;
}

tree_code tree_insert_between_operations(Diff_tree* dtree, long long *index_above, operations op)
{
    if (*index_above == ABSENT)
    {
        ASSERTION(TREE_UNDERFLOW);
        tree_dump(dtree, TREE_UNDERFLOW, REMOVING);
        return TREE_UNDERFLOW;
    }

    long long parent = tree_parent(dtree, *index_above);
    char was_left = (*index_above == tree_left_son(dtree, parent));
    if (was_left)
        dtree->tree[parent].left_son = ABSENT;
    else
        dtree->tree[parent].right_son = ABSENT;

    tree_insert_operation(dtree, &parent, op);
    dtree->tree[parent].left_son = *index_above;
    *index_above = parent;

    return TREE_OK;
}

tree_code tree_check_pointer(Diff_tree* dtree)
{
    if (!dtree || !dtree->tree)
        return TREE_NULL;

    if ((size_t)(dtree) <= 4096 || (size_t)(dtree->tree) <= 4096)
        return TREE_SEG_FAULT;

    return TREE_OK;
}

void tree_check_connections(Diff_tree* dtree, long long index, tree_code* error)
{
    long long parent = tree_parent(dtree, index);
    long long right = tree_right_son(dtree, index);
    long long left = tree_left_son(dtree, index);
    if (right == ABSENT && left == ABSENT)
        return;
    else if (right < ABSENT || left < ABSENT)
    {
        *error = TREE_CONNECT_ERROR;
        return;
    }

    if (right != ABSENT)
    {
        if (index != tree_parent(dtree, right))
        {
            *error = TREE_CONNECT_ERROR;
            return;
        }

        tree_check_connections(dtree, right, error);
    }
    if (left != ABSENT)
    {
        if (index != tree_parent(dtree, left))
        {
            *error = TREE_CONNECT_ERROR;
            return;
        }

        tree_check_connections(dtree, left, error);
    }
}

void tree_print_errors(tree_code code)
{
    printf("Error: %s\n", tree_state_text[code]);
}

void tree_dump(Diff_tree* dtree, tree_code code, const tree_functions function)
{
    static long int doing = 0;

    const char* mode = "";
    if (!doing)
        mode = "wb";
    else
        mode = "ab";
    FILE* log = fopen("tree_appearance.html", mode);

    fprintf(log, "<pre><font size=\"5\"  face=\"Times New Roman\">\n");

    fprintf(log, "<p><span style=\"font-weight: bold\">CURRENT STATE OF TREE</span></p>\n");
    fprintf(log, "THE NEWS FROM %s\n", TREE_FUNCTION_IDENTIFIERS[function]);

    fprintf(log, "%s", tree_state_text[code]);

    fprintf(log, "CURRENT CAPACITY IS %lld\n", dtree->capacity);
    fprintf(log, "CURRENT SIZE IS            %lld\n", dtree->size);

    fprintf(log, "</font><tt>\n");

    char picture_name[MAX_PICTURE_NAME] = " ";

    sprintf(picture_name, "%s%ld.%s", PICTURE_NAME, doing, PICTURE_CODE_EXPANSION);

    tree_print_picture(dtree, picture_name);

    strncat(picture_name, ".", MAX_PICTURE_NAME);
    strncat(picture_name, PICTURE_EXPANSION, MAX_PICTURE_NAME);

    fprintf(log, "<img src=%s>", picture_name);

    fprintf(log, "\n");

    fclose(log);

    doing++;
}

tree_code tree_verifier(Diff_tree* dtree, const tree_functions requester)
{
    tree_code error = tree_check_pointer(dtree);
    if (error != TREE_OK)
    {
        ASSERTION(error);
        tree_dump(dtree, error, requester);
        return error;
    }

    long long  connections_number = dtree->capacity - dtree->size;
    long long* free_indexes = (long long*)calloc(dtree->capacity, sizeof(long long));

    long long index = dtree->first_free;
    for (int i = 0; i < connections_number; i++)
    {
        if (free_indexes[index])
        {
            ASSERTION(TREE_CONNECT_ERROR);
            tree_dump(dtree, TREE_CONNECT_ERROR, requester);
            return TREE_CONNECT_ERROR;
        }
        free_indexes[index]++;

        index = dtree->tree[index].next;
    }
    free(free_indexes);

    if (dtree->tree[dtree->root_index].parent != ABSENT)
    {
        ASSERTION(TREE_CONNECT_ERROR);
        tree_dump(dtree, TREE_CONNECT_ERROR, requester);
        return TREE_CONNECT_ERROR;
    }

    tree_check_connections(dtree, dtree->root_index, &error);
    if (error != TREE_OK)
    {
        ASSERTION(TREE_CONNECT_ERROR);
        tree_dump(dtree, TREE_CONNECT_ERROR, requester);
        return TREE_CONNECT_ERROR;
    }

    return TREE_OK;
}

void tree_dot_call(const char* name_file, const char* expansion)
{
    char temp[512];
    sprintf(temp, "dot %s -T%s -O", name_file, expansion);
    system((char*)temp);
}

const char* tree_operations[] =
{
    "1",
    "1",
    "+",
    "-",
    "*",
    "/",
    "^",
    "sin",
    "cos",
    "ln",
    "sqrt",
    "and",
    "or"
};

const char* tree_comparisons[] =
{
    "More",
    "Less",
    "Equal",
    "!"
};

const char* tree_key_words[] =
{
    "Func",
    "let",
    "(",
    ")",
    "while",
    "if",
    "double",
    "=",
    "return",
    "print",
    "scan",
    "with",
    "inside",
    "it's",
    "what",
    "that",
    "having",
    "at_home"
};



void tree_print_node(Diff_tree* dtree, long long index, FILE* picture)
{
    if (dtree->tree[index].type == NUMBER)
        fprintf(picture, "%lg", dtree->tree[index].number);
    else if (dtree->tree[index].type == VARIABLE || dtree->tree[index].type == FUNC)
    {
        wchar_t var[MAX_VAR_NAME] = {};
        wmemcpy(var, dtree->tree[index].variable, dtree->tree[index].variable_length);
        //fprintf(picture, "%s", var);
        if (dtree->tree[index].type == VARIABLE)
            fprintf(picture, "var");
        else
            fprintf(picture, "func");
    }
    else if (dtree->tree[index].type == OPERATION)
        fprintf(picture, "oper %s", tree_operations[dtree->tree[index].operation], dtree->tree[index].operation);
    else if (dtree->tree[index].type == COMPARISON)
        fprintf(picture, "comp %s", tree_comparisons[dtree->tree[index].comp], dtree->tree[index].comp);
    else if (dtree->tree[index].type == KEY_WORD)
        fprintf(picture, "key %s, %d", tree_key_words[dtree->tree[index].key], dtree->tree[index].key);
    else if (dtree->tree[index].type == PARAM)
        fprintf(picture, "param %d", dtree->tree[index].func_params->size);
    else
        fprintf(picture, "Linker");
}

void tree_print_picture_nodes(Diff_tree* dtree, long long index, FILE* picture)
{
#ifdef DEBUG
    fprintf(picture, "  nod%lld[shape=\"none\", ", index);
    fprintf(picture, "label = <<table border = \"0\" cellborder = \"1\" cellspacing = \"0\">\n");
    fprintf(picture, "      <tr>\n      <td colspan = \"2\" bgcolor = \"HotPink\">parent %lld</td>\n    </tr>\n", dtree->tree[index].parent);
    fprintf(picture, "      <tr>\n      <td colspan = \"2\" bgcolor = \"%s\">index %lld</td>\n     </tr>\n", COLOR, index);
    fprintf(picture, "      <tr>\n      <td colspan = \"2\" bgcolor = \"%s\">type %ld</td>\n     </tr>\n", COLOR, dtree->tree[index].type);
    fprintf(picture, "      <tr>\n      <td colspan = \"2\" bgcolor = \"%s\">", COLOR);
    tree_print_node(dtree, index, picture);
    fprintf(picture, "</td>\n     </tr>\n");
    fprintf(picture, "      <tr>\n");
    fprintf(picture, "          <td bgcolor = \"%s\">%lld</td>\n", COLOR_OTHER, dtree->tree[index].left_son);
    fprintf(picture, "          <td bgcolor = \"%s\">%lld</td>\n", COLOR_OTHER, dtree->tree[index].right_son);
    fprintf(picture, "      </tr>\n");
    fprintf(picture, "      </table>>];\n");
#else
    fprintf(picture, "  nod%lld[shape=\"none\", ", index);
    fprintf(picture, "fillcolor =  \"%s\", style=filled", COLOR);
    fprintf(picture, "  label = \"");
    tree_print_node(dtree, index, picture); 
    fprintf(picture, "\"];\n");
#endif
}

void tree_generate_picture_nodes(Diff_tree* dtree, long long index, FILE* picture)
{
    long long right = tree_right_son(dtree, index);
    long long left  = tree_left_son(dtree, index);
    if (left < 0 && right < 0)
    {
        tree_print_picture_nodes(dtree, index, picture);

        return;
    }

    tree_print_picture_nodes(dtree, index, picture);

    if (left >= 0)
    {
        tree_generate_picture_nodes(dtree, left, picture);
    }
    if (right >= 0)
    {
        tree_generate_picture_nodes(dtree, right, picture);
    }

    return;
}

void tree_print_picture_sequence(Diff_tree* dtree, long long index, FILE* picture)
{
    long long right = tree_right_son(dtree, index);
    long long left = tree_left_son(dtree, index);
    if (left < 0 && right < 0)
    {
        return;
    }

    if (left != ABSENT)
    {
        tree_print_picture_sequence(dtree, left, picture);
        fprintf(picture, "      nod%lld->nod%lld\n", index, left);
    }
    if (right != ABSENT)
    {
        tree_print_picture_sequence(dtree, right, picture);
        fprintf(picture, "      nod%lld->nod%lld\n", index, right);
    }

    return;
}

void tree_print_picture(Diff_tree* dtree, const char* picture_name)
{
    FILE* picture = fopen(picture_name, "wb");

    fprintf(picture, "digraph\n{\n  rankdir = TB;\n");
    tree_generate_picture_nodes(dtree, dtree->root_index, picture);
    fprintf(picture, "   ");
    tree_print_picture_sequence(dtree, dtree->root_index, picture);
    fprintf(picture, "}");

    fclose(picture);

    tree_dot_call(picture_name, PICTURE_EXPANSION);
}






Param* param_new(size_t amount)
{
    Param* new_one = (Param*)calloc(1, sizeof(Param));

    if (!new_one)
        return NULL;

    param_construct(new_one, amount);
    if (!(new_one->params))
    {
        param_delete(new_one);
        return NULL;
    }

    return new_one;
}

void param_construct(Param* thus, size_t amount)
{
    thus->params = (Var*)calloc(amount, sizeof(Var));

    thus->capacity = amount;
    thus->size = 0;
}

void param_delete(Param* thus)
{
    param_destruct(thus);

    free(thus);
}

void param_destruct(Param* thus)
{
    if (thus && thus->params)
    {
        free(thus->params);
        thus->params = NULL;
    }
}

void param_resize(Param* thus)
{
    size_t new_cap = thus->capacity * 2;

    Var* new_pointer = (Var*)realloc(thus->params, new_cap * sizeof(Var));

    thus->params = new_pointer;
    thus->capacity = new_cap;
}

void param_insert(Param* thus, wchar_t* var, size_t length)
{
    printf("thus %p\n", thus);
    if (thus->size + 1 >= thus->capacity)
        param_resize(thus);

    thus->params[thus->size].variable = var;
    thus->params[thus->size].variable_length = length;

    thus->size++;
}
