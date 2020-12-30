#include "code_tree_builder.h"

/*
Есть массив
Идём по массиву 

Пока не говорит, где ошибки
*/

const wchar_t* code_operations[] =
{
	L"трошки",
	L"трошки",
	L"+",
	L"-",
	L"*",
	L"/",
	L"^",
	L"sin",
	L"cos",
	L"ln",
	L"sqrt"
};

const wchar_t* code_comparisons[] =
{
	L">",
	L"<",
	L"=",
	L"!"
};

const wchar_t* code_key_words[] =
{
	L"хата",
	L"хай",
	L"(",
	L")",
	L"пока_що",
	L"якщо",
	L"буде",
	L"як",
	L"тiкае_звiдси",
	L"побачит",
	L"намалюэ",
	L"з",
	L"всерединi",
	L"це",
	L"шо_це_таке",
	L"а_ось_таке",
	L"маэ",
	L"в хате",
	L"викличе"
};



Prog_state* prog_new(Code* code, Diff_tree* tree)
{
	Prog_state* new_prog = (Prog_state*)calloc(1, sizeof(Prog_state));
	new_prog->code = code;
	new_prog->current_index = 0;

	if (tree)
		new_prog->tree = tree;
	else
		new_prog->tree = tree_init(10);
	new_prog->tree_index = ABSENT;

	new_prog->in_amount = 0;

	return new_prog;
}

void prog_delete(Prog_state* thus)
{
	code_delete(thus->code);
	//tree_delete(thus->tree);
	free(thus);
}

void syntax_error(const wchar_t *wrong_symbol)
{
	wprintf(L"Syntax error: %ls\n", wrong_symbol);
	//abort();
}


void require_key(Prog_state* state, key_word op)
{
	if (state->code->array[state->current_index].type != KEY_WORD || state->code->array[state->current_index].key != op)
	{
		state->tree_index = ABSENT;
		syntax_error(L"Not is the key or the other key required");
		printf("index is %d, I wanted %d\n", state->current_index, op);
	}
	else
	{
		wprintf(L"Всё хорошо: %ls\n", code_key_words[op]);
		state->current_index++;
	}
}

char require_separator(Prog_state* state)
{
	if (state->code->array[state->current_index].type != KEY_WORD || state->code->array[state->current_index].key != KEY_SEPARATOR)
		return 0;
	else
	{
		state->current_index++;
		return 1;
	}
}

Diff_tree *code_get_general(Code* input)
{
	Prog_state* state = prog_new(input, NULL);

	tree_insert_linker(state->tree, &(state->tree_index));
	
	state->tree_index = code_get_func(state);
	
	tree_insert_linker(state->tree, &(state->tree_index));
	
	while (state->current_index < state->code->size)
	{
		state->tree_index = code_get_func(state);
		if (state->tree_index == ABSENT)
			break;
		
		tree_insert_linker(state->tree, &(state->tree_index));
	}
	tree_dump(state->tree, TREE_OK, CONSTRUCTION);

	Diff_tree* answer = state->tree;
	prog_delete(state);
	if (state->tree_index == ABSENT)
		return NULL;
	return answer;
}

long long code_get_func(Prog_state* state)
{	
	require_key(state, KEY_FUNCTION);
	
	code_get_identifier(state, FUNC);
	state->tree_index     = tree_left_son(state->tree, state->tree_index);
	long long start_index = state->tree_index;
	
	code_get_var(state, 0);
	if (state->code->array[state->current_index].type == KEY_WORD
	 && state->code->array[state->current_index].key == KEY_PARAM)
	{
		require_key(state, KEY_PARAM);
		code_get_var(state, 1);
		while (state->code->array[state->current_index].type == KEY_WORD
			&& state->code->array[state->current_index].key == KEY_PARAM)
		{
			require_key(state, KEY_PARAM);
			code_get_var(state, 1);
		}
		require_key(state, KEY_END_PARAM);
	}
	
	state->tree_index = start_index;
	
	tree_insert_linker(state->tree, &(state->tree_index));

	state->in_amount++;
	state->tree_index = code_get_block(state);
	state->in_amount--;

	if (state->tree_index == ABSENT)
		return ABSENT;
	
	return tree_go_up(state->tree, state->tree_index, ONE_STEP);
}

char code_get_number(Prog_state* state)
{
	double val = 0;
	if (state->code->array[state->current_index].type == NUMBER)
	{
		tree_insert_number(state->tree, &(state->tree_index), state->code->array[state->current_index].number);
		state->tree_index = tree_parent(state->tree, state->tree_index);
		state->current_index++;
	}
	else
		return 0;

	return 1;
}

char code_get_var(Prog_state* state, int amount)
{
	if (!amount)
	{
		tree_insert_params(state->tree, &(state->tree_index));
		state->tree_index = tree_parent(state->tree, state->tree_index);
	}
	else if (state->code->array[state->current_index].type == VARIABLE)
	{
		state->tree_index = tree_left_son(state->tree, state->tree_index);

		wchar_t* var = state->code->array[state->current_index].variable;
		size_t var_len = state->code->array[state->current_index].variable_length;

		wchar_t varr[MAX_VAR_NAME] = {};
		wmemcpy(varr, var, var_len);
		wprintf(L"Да какого чёрта ты не работаешь? %ls\n", varr);

		printf("func params %lld\n", state->tree_index);
		param_insert(state->tree->tree[state->tree_index].func_params, var, var_len);
		state->tree_index = tree_parent(state->tree, state->tree_index);

		state->current_index++;
	}
	else	
		return 0;

	return 1;
}

char code_get_identifier(Prog_state *state, const char which_id)
{
	if (state->code->array[state->current_index].type == VARIABLE)
	{
		wchar_t* var = state->code->array[state->current_index].variable;
		size_t var_len = state->code->array[state->current_index].variable_length;
		if (which_id == FUNC)
		{
			tree_insert_function(state->tree, &(state->tree_index), var, var_len);
		}
		else
		{
			tree_insert_variable(state->tree, &(state->tree_index), var, var_len);
		}
		state->current_index++;
		state->tree_index = tree_parent(state->tree, state->tree_index);
	}
	else if (state->code->array[state->current_index].type == KEY_WORD
		&& state->code->array[state->current_index].key == KEY_CALLING)
	{
		require_key(state, KEY_CALLING);

		wchar_t* var = state->code->array[state->current_index].variable;
		size_t var_len = state->code->array[state->current_index].variable_length;
		tree_insert_function(state->tree, &(state->tree_index), var, var_len);
		long long returned_index = state->tree_index;

		tree_insert_number(state->tree, &(state->tree_index), ABSENT);
		state->tree_index = tree_go_up(state->tree, state->tree_index, ONE_STEP);

		tree_insert_linker(state->tree, &(state->tree_index));

		if (state->current_index < state->code->size)
		{
			if (state->code->array[state->current_index + 1].type == KEY_WORD
				&& state->code->array[state->current_index + 1].key == KEY_LEFT_BRACKET)
			{
				state->current_index++;
				require_key(state, KEY_LEFT_BRACKET);
				state->tree_index = code_get_logical_and(state);
				tree_insert_linker(state->tree, &(state->tree_index));
				//code_get_var(state, 0);
				while (state->code->array[state->current_index].type == KEY_WORD
					&& state->code->array[state->current_index].key == KEY_PARAM)
				{
					require_key(state, KEY_PARAM);
					state->tree_index = code_get_logical_and(state);
					tree_insert_linker(state->tree, &(state->tree_index));
					//code_get_var(state, 1);
				}
				require_key(state, KEY_RIGHT_BRACKET);
			}
		}

		state->tree_index = returned_index;
	}
	else
		return 0;

	return 1;
}

operations code_get_unary_operations(Prog_state* state)
{
	if (state->code->array[state->current_index].type == OPERATION)
	{
		operations op = state->code->array[state->current_index].operation;

		if (op > OP_POW && op < OP_AND)
		{
			tree_insert_operation(state->tree, &(state->tree_index), op);

			tree_insert_number(state->tree, &(state->tree_index), ABSENT);
			state->tree_index = tree_parent(state->tree, state->tree_index);

			state->current_index++;

			return op;
		}
		else
		{
			state->tree_index = ABSENT;
			syntax_error(L"Wrong operation before identifier or number");
		}
	}
	else
		return NO_OP;
}

long long code_get_priority(Prog_state* state)
{
	if (state->code->array[state->current_index].type == KEY_WORD
		&& state->code->array[state->current_index].key == KEY_OPEN_PRIORITY)
	{
		state->current_index++;

		state->tree_index = code_get_expression(state);//Получаем индекс и перепривешиваем
		require_key(state, KEY_CLOSE_PRIORITY);
	}
	else
	{
		operations op = code_get_unary_operations(state);

		if (state->code->array[state->current_index].type == KEY_WORD
			  && state->code->array[state->current_index].key == KEY_CALLING)
			code_get_identifier(state, FUNC);
		else if (!code_get_number(state))
		if (!code_get_identifier(state, VARIABLE))
		{
			state->tree_index = ABSENT;
			syntax_error(L"No term found");
		}

		if (op != NO_OP)
		{
			state->tree_index = tree_parent(state->tree, state->tree_index);
		}
	}
	
	return state->tree_index;
}

long long code_get_term(Prog_state* state)
{
	size_t amount = 0;
	long long curr_index = state->tree_index;
	int way = (tree_left_son(state->tree, state->tree_index) == ABSENT);
	state->tree_index = code_get_priority(state);

	if (way)
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	else
		state->tree_index = tree_right_son(state->tree, state->tree_index);
	
	while (state->tree_index != ABSENT && state->code->array[state->current_index].type == OPERATION
		&&(state->code->array[state->current_index].operation == OP_MUL || state->code->array[state->current_index].operation == OP_DIV))
	{
		operations op = state->code->array[state->current_index].operation;
		
		//перепривешиваем
		tree_insert_between_operations(state->tree, &(state->tree_index), op);
		state->current_index++;

		state->tree_index = code_get_priority(state);
		amount++;
	}

	if (state->tree_index == ABSENT)
		return ABSENT;
	return curr_index;
}

long long code_get_expression(Prog_state* state)
{
	size_t amount = 0;
	long long curr_index = state->tree_index;
	int way = (tree_left_son(state->tree, state->tree_index) == ABSENT);
	
	state->tree_index = code_get_term(state);
	
	if (way)
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	else
		state->tree_index = tree_right_son(state->tree, state->tree_index);

	//state->tree_index = curr_index;
	while (state->tree_index != ABSENT && state->code->array[state->current_index].type == OPERATION
	    &&(state->code->array[state->current_index].operation == OP_PLUS || state->code->array[state->current_index].operation == OP_SUB))
	{
		operations op = state->code->array[state->current_index].operation;
			
		//получили новый плюс или минус? перепривешиваем
		tree_insert_between_operations(state->tree, &(state->tree_index), op);
		//if (!amount) curr_index = state->tree_index;
		state->current_index++;
	
		state->tree_index = code_get_term(state);
		amount++;
	}

	if (state->tree_index == ABSENT)
		return ABSENT;
	return curr_index;
}

long long code_get_math_comparison(Prog_state* state)
{
	long long curr = state->tree_index;
	
	int way = (tree_left_son(state->tree, state->tree_index) == ABSENT);
	state->tree_index = code_get_expression(state);

	if (way)
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	else
		state->tree_index = tree_right_son(state->tree, state->tree_index);

	if (state->code->array[state->current_index].type == COMPARISON)
	{
		tree_insert_between_comparison(state->tree, &(state->tree_index), state->code->array[state->current_index].comp);
		state->current_index++;
		state->tree_index = code_get_expression(state);
	}
	
	if (state->tree_index == ABSENT)
		return ABSENT;
	return curr;
}

long long code_get_logical_or(Prog_state* state)
{
	size_t amount = 0;
	long long curr_index = state->tree_index;
	int way = (tree_left_son(state->tree, state->tree_index) == ABSENT);
	state->tree_index = code_get_math_comparison(state);

	if (way)
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	else
		state->tree_index = tree_right_son(state->tree, state->tree_index);

	while (state->tree_index != ABSENT && state->code->array[state->current_index].type == OPERATION
		&&(state->code->array[state->current_index].operation == OP_OR))
	{
		operations op = state->code->array[state->current_index].operation;

		tree_insert_between_operations(state->tree, &(state->tree_index), op);
		state->current_index++;

		state->tree_index = code_get_math_comparison(state);
		amount++;
	}

	if (state->tree_index == ABSENT)
		return ABSENT;
	return curr_index;
}

long long code_get_logical_and(Prog_state* state)
{
	size_t amount = 0;
	long long curr_index = state->tree_index;
	int way = (tree_left_son(state->tree, state->tree_index) == ABSENT);
	
	state->tree_index = code_get_logical_or(state);

	if (way)
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	else
		state->tree_index = tree_right_son(state->tree, state->tree_index);

	while (state->tree_index != ABSENT && state->code->array[state->current_index].type == OPERATION
		&& (state->code->array[state->current_index].operation == OP_AND))
	{
		operations op = state->code->array[state->current_index].operation;

		tree_insert_between_operations(state->tree, &(state->tree_index), op);
		state->current_index++;

		state->tree_index = code_get_logical_or(state);
		amount++;
	}

	if (state->tree_index == ABSENT)
		return ABSENT;
	return curr_index;
}

long long code_get_block(Prog_state* state)
{
	size_t amount = 0;
	while (state->current_index < state->code->size && state->code->array[state->current_index].type == KEY_WORD
		&& state->code->array[state->current_index].key == KEY_SEPARATOR)
	{
		for (size_t i = 0; i < state->in_amount; i++)
		{
			if (!require_separator(state) || state->current_index >= state->code->size)
			{
				state->current_index -= i;
				state->tree_index = tree_go_up(state->tree, state->tree_index, amount + 1);
				return state->tree_index;
			}
		}

		state->tree_index = code_get_line(state);

		if (state->tree_index == ABSENT)
			return ABSENT;
		
		tree_insert_linker(state->tree, &(state->tree_index));

		amount++;
	}
	
	state->tree_index = tree_go_up(state->tree, state->tree_index, amount + 1);

	return state->tree_index;
}

long long code_get_declaration(Prog_state* state)
{
	tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index].key);
	tree_insert_number(state->tree, &(state->tree_index), ABSENT);
	state->tree_index = tree_go_up(state->tree, state->tree_index, ONE_STEP);
	state->current_index++;
	if (!code_get_identifier(state, VARIABLE))
	{
		state->tree_index = ABSENT;
		syntax_error(L"No variable");
	}

	return state->tree_index;
}

long long code_get_assignment(Prog_state* state)
{
	wchar_t* var_pointer = state->code->array[state->current_index].variable;
	size_t var_length = state->code->array[state->current_index].variable_length;

	state->current_index++;
	require_key(state, KEY_ASSIGN);

	tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index - 1].key);

	tree_insert_variable(state->tree, &(state->tree_index), var_pointer, var_length);

	state->tree_index = tree_parent(state->tree, state->tree_index);

	if (state->code->array[state->current_index].type == KEY_WORD && state->code->array[state->current_index].key == KEY_SCAN)
	{
		require_key(state, KEY_SCAN);
		tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index].key);
		//state->current_index++;
		return tree_parent(state->tree, state->tree_index);
		//return state->tree_index;
	}

	return code_get_logical_and(state);
}

long long code_get_unary(Prog_state* state)
{
	if (state->code->array[state->current_index].key == KEY_PRINT)
		require_key(state, KEY_PRINT);
	else
		require_key(state, KEY_RETURN);

	tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index - 1].key);
	tree_insert_number(state->tree, &(state->tree_index), ABSENT);
	state->tree_index = tree_go_up(state->tree, state->tree_index, ONE_STEP);

	return code_get_logical_and(state);
}

long long code_get_condition_block(Prog_state* state)
{
	tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index].key);
	state->current_index++;

	state->tree_index = code_get_logical_and(state);
	tree_dump(state->tree, TREE_OK, CONSTRUCTION);

	tree_insert_linker(state->tree, &(state->tree_index));

	state->in_amount++;
	state->tree_index = code_get_block(state);
	state->in_amount--;

	return state->tree_index;
}

long long code_get_line(Prog_state* state)
{
	if (state->code->array[state->current_index].type == KEY_WORD && state->code->array[state->current_index].key == KEY_DECL)
	{
		state->tree_index = code_get_declaration(state);
	}
	else if (state->code->array[state->current_index].type == VARIABLE && state->code->array[state->current_index + 1].type == KEY_WORD
		  && state->code->array[state->current_index + 1].key == KEY_ASSIGN)
	{
		state->tree_index = code_get_assignment(state);
	}
	else if (state->code->array[state->current_index].type == KEY_WORD 
		 && (state->code->array[state->current_index].key == KEY_PRINT || state->code->array[state->current_index].key == KEY_RETURN))
	{
		state->tree_index = code_get_unary(state);
	}
	else if (state->code->array[state->current_index].type == KEY_WORD
		 && (state->code->array[state->current_index].key == KEY_WHILE || state->code->array[state->current_index].key == KEY_IF))
	{
		state->tree_index = code_get_condition_block(state);
	}
	else if (state->code->array[state->current_index].type == KEY_WORD && state->code->array[state->current_index].key == KEY_SCAN)
	{
		tree_insert_key_word(state->tree, &(state->tree_index), state->code->array[state->current_index].key);
		state->tree_index = tree_parent(state->tree, state->tree_index);
	}
	else
	{
		state->tree_index = code_get_logical_and(state);
		state->tree_index = tree_left_son(state->tree, state->tree_index);
	}

	if (state->tree_index == ABSENT)
		return ABSENT;
	
	state->tree_index = tree_go_up(state->tree, state->tree_index, ONE_STEP);
	return state->tree_index;
}