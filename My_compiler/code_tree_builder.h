#pragma once

#include "lexical_analyzer.h"
#include "diff_tree.h"

typedef struct Current_program_state
{
	Code* code;
	size_t current_index;

	Diff_tree* tree;
	long long tree_index;

	long long in_amount;
}Prog_state;

Prog_state* prog_new					(Code* code, Diff_tree *tree);
void		prog_delete					(Prog_state* thus);


void		syntax_error				(const wchar_t* wrong_symbol);

void		require_key					(Prog_state* state, key_word op);

Diff_tree  *code_get_general			(Code* input);

long long	code_get_func				(Prog_state* state);

char		code_get_number				(Prog_state* state);
char		code_get_var				(Prog_state* state, int amount);
char		code_get_identifier			(Prog_state* state, const char which_id);

long long	code_get_priority			(Prog_state* state);
long long	code_get_term				(Prog_state* state);
long long	code_get_expression			(Prog_state* state);
long long	code_get_math_comparison	(Prog_state* state);
long long	code_get_logical_or			(Prog_state* state);
long long	code_get_logical_and		(Prog_state* state);

long long	code_get_block				(Prog_state* state);

long long	code_get_declaration		(Prog_state* state);
long long	code_get_assignment			(Prog_state* state);
long long	code_get_unary				(Prog_state* state);
long long	code_get_condition_block	(Prog_state* state);

long long	code_get_line				(Prog_state* state);