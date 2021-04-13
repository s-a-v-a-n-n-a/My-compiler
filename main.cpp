#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "lexical_analyzer.h"
#include "code_tree_builder.h"
#include "generating_asm_code.h"

char* main_string = NULL;

/*void syntax_error(char wrong_symbol);
void require(char symbol);
int get_general(char* input_string);
int get_number();
int get_priority();
int get_term();
int get_expression();*/

void compilation_assertion(Compilation_errors code);

int main()
{
	setlocale(LC_CTYPE, "Russian");
	
	Code* new_one = code_file_reader("square.txt");
	if (!new_one)
	{
		compilation_assertion(LEXICAL_ERROR);
		return 0;
	}
	Diff_tree *codes = code_get_general(new_one);
	if (!codes)
	{
		compilation_assertion(PARSER_ERROR);
		return 0;
	}
	int returning = code_generation(codes);
	if (returning)
	{
		compilation_assertion(COMPILATION_ERROR);
		return 0;
	}

	return 0;
}

void compilation_assertion(Compilation_errors code)
{
	switch (code)
	{
	case LEXICAL_ERROR:
		printf("Error: WRONG LEXICS IN CODE\n\n");
		break;
	case PARSER_ERROR:
		printf("Error: WRONG LEXICS SEQUENSE\n\n");
		break;
	case COMPILATION_ERROR:
		printf("Error: USAGE OF NON-EXISTENT VARIABLES\n\n");
		break;
	default:
		break;
	}
}

/*void syntax_error(char wrong_symbol)
{
	printf("Syntax error from symbol %d\n", wrong_symbol);
	abort();
}

void require(char symbol)
{
	if (*main_string != symbol)
		syntax_error(symbol);
	else
		main_string++;
}

int get_general(char* input_string)
{
	main_string = input_string;
	int val = get_expression();
	require('\0');
	main_string++;

	return val;
}

int get_number()
{
	printf("%d\n", *main_string);
	int val = 0;
	while ('0' <= *main_string && *main_string <= '9')
	{
		val = val*10 + *main_string - '0';
		main_string++;
	}
	//else
		//syntax_error(*main_string);

	return val;
}

int get_priority()
{
	int val = 0;
	if (*main_string == '(')
	{
		main_string++;
		val = get_expression();
		require(')');
		//main_string++;
	}
	else
		val = get_number();

	return val;
}

int get_term()
{
	int val = get_priority();
	while (*main_string == '*' || *main_string == '/')
	{
		int op = *main_string;
		main_string++;

		int next_val = get_priority();
		if (op == '*')
			val *= next_val;
		else
			val /= next_val;
	}


	return val;
}

int get_expression()
{
	int val = get_term();
	if (*main_string == '+' || *main_string == '-')
	{
		while (*main_string == '+' || *main_string == '-')
		{
			int op = *main_string;
			main_string++;

			int next_val = get_term();
			if (op == '+')
				val += next_val;
			else
				val -= next_val;
		}
	}
	//else
		//syntax_error(*main_string);

	return val;
}
*/