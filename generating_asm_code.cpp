#define _CRT_SECURE_NO_WARNINGS

#include "generating_asm_code.h"

Frame* frame_new(size_t amount, char type)
{
	Frame* new_frame = (Frame*)calloc(1, sizeof(Frame));

	if (!new_frame)
		return NULL;
	
	if (frame_construct(new_frame, amount, type) != FRAME_OK)
	{
		frame_delete(new_frame);
		return NULL;
	}

	return new_frame;
}

frame_code frame_construct(Frame *thus, size_t amount, char type)
{
	thus->vars = (Var*)calloc(amount, sizeof(Var));
	if (!thus->vars)
		return FRAME_NO_CONSTRUCTION;

	thus->vars_amount = 0;
	thus->vars_capacity = amount;

	thus->frame_type = type;

	thus->current_pointer = NULL;

	return FRAME_OK;
}

frame_code frame_delete(Frame* thus)
{
	frame_destruct(thus);
	free(thus);

	return FRAME_OK;
}

frame_code frame_destruct(Frame* thus)
{
	if (thus->vars)
	{
		free(thus->vars);
		thus->vars = NULL;
	}

	return FRAME_OK;
}

frame_code frame_resize(Frame* thus)
{
	size_t new_cap = thus->vars_capacity * 2;

	Var* new_pointer = (Var*)realloc(thus->vars, new_cap * sizeof(Var));
	if (!new_pointer)
	{
		return FRAME_NO_MEMORY;
	}
	
	thus->vars = new_pointer;
	thus->vars_capacity = new_cap;

	return FRAME_OK;
}

//Прикрутить проверку на ошибки
frame_code frame_insert(Frame* thus, wchar_t* var, size_t length, size_t args, size_t address)
{
	if (thus->vars_amount + 1 >= thus->vars_capacity)
	{
		frame_code error = frame_resize(thus);
		if (error != FRAME_OK)
			return error;
	}
	
	thus->vars[thus->vars_amount].variable = var;
	thus->vars[thus->vars_amount].variable_length = length;

	thus->vars[thus->vars_amount].address = address;
	thus->vars[thus->vars_amount].args_amount = args;

	thus->vars_amount++;

	return FRAME_OK;
}


Compiler* compiler_new(Diff_tree *tree)
{
	Compiler* new_comp = (Compiler*)calloc(1, sizeof(Compiler));

	new_comp->asm_code = fopen(ASM_FILE_NAME, "wb");
	new_comp->code_tree = tree;

	fprintf(new_comp->asm_code, "CALL #");
	print_const_russian_name(new_comp->asm_code, L"президента");
	fprintf(new_comp->asm_code, "\nHLT\n\n");

	new_comp->scopes = stack_new(MIN_STACK_SIZE);

	Frame* new_one = frame_new(MIN_FRAME_SIZE, FUNCTION_FRAME);
	Structure* current = new_comp->scopes->stack;

	long long root = tree->root_index;
	long long ram = 0;
	while (!tree_if_lief(tree, root))
	{
		long long index = tree_left_son(tree, root);
		printf("%lld\n", index);
		
		wchar_t* var = tree->tree[index].variable;
		size_t var_len = tree->tree[index].variable_length;
		size_t param_amount = 0;
		long long func_left = tree_left_son(tree, index);
		if (tree->tree[func_left].func_params)
		{
			param_amount = tree->tree[func_left].func_params->size;
		}
		
		frame_insert(new_one, var, var_len, param_amount, ram);
		wchar_t var_to_write[MAX_VAR_NAME] = {};
		wmemcpy(var_to_write, var, var_len);
		//wprintf(L"address %lu, %ls\n", new_one->vars[new_one->vars_amount - 1].address, var_to_write);

		ram++;
		root = tree_right_son(tree, root);
	}

	stack_push(&new_comp->scopes, new_one);

	new_comp->number_of_conditional_jump = 0;
	new_comp->unused_values_amount       = 0;
	new_comp->index                      = new_comp->code_tree->root_index;

	return new_comp;
}

void compiler_delete(Compiler* thus)
{
	Frame *last = frame_new(MIN_FRAME_SIZE, FUNCTION_FRAME);
	stack_pop(&thus->scopes, &last);
	frame_delete(last);

	stack_destruct(&thus->scopes);

	fclose(thus->asm_code);

	tree_delete(thus->code_tree);

	free(thus);
}



int strings_comparison(wchar_t* var_1, size_t length_1, wchar_t* var_2, size_t length_2)
{
	if (length_1 != length_2)
		return 0;
	
	wchar_t first_var_to_compare[MAX_VAR_NAME] = {};
	wmemcpy(first_var_to_compare, var_1, length_1);

	wchar_t second_var_to_compare[MAX_VAR_NAME] = {};
	wmemcpy(second_var_to_compare, var_2, length_2);

	if (wcscmp(first_var_to_compare, second_var_to_compare))
		return 0;
	else
		return 1;
}

long long find_this_var(Frame* frame, wchar_t* var, size_t length)
{
	wchar_t varr[MAX_VAR_NAME] = {};
	wmemcpy(varr, var, length);
	wprintf(L"finding %ls\n", varr);
	for (size_t i = 0; i < frame->vars_amount; i++)
	{
		/*wchar_t varrr[MAX_VAR_NAME] = {};
		wmemcpy(varrr, frame->vars[i].variable, frame->vars[i].variable_length);
		wprintf(L"address %ls\n", varrr);*/

		
		if (strings_comparison(var, length, frame->vars[i].variable, frame->vars[i].variable_length))
			return i;
	}

	return ABSENT;
}

void print_var(Diff_tree* tree, long long index)
{
	wchar_t var[MAX_VAR_NAME] = {};
	wmemcpy(var, tree->tree[index].variable, tree->tree[index].variable_length);
	wprintf(L"%ls\n", var);
}

void print_params(Diff_tree* tree, long long index)
{
	index = tree_left_son(tree, index);
	for (size_t i = 0; i < tree->tree[index].func_params->size; i++)
	{
		wchar_t var[MAX_VAR_NAME] = {};
		wmemcpy(var, tree->tree[index].func_params->params[i].variable, tree->tree[index].func_params->params[i].variable_length);
		wprintf(L"всерединi хаты %ls\n", var);
	}
}

void insert_params(Frame *frames, Diff_tree* tree, long long index, long long *ram)
{
	index = tree_left_son(tree, index);
	for (size_t i = 0; i < tree->tree[index].func_params->size; i++)
	{
		frame_insert(frames, tree->tree[index].func_params->params[i].variable, tree->tree[index].func_params->params[i].variable_length, tree->tree[index].func_params->size, *ram);
		/*wchar_t var[MAX_VAR_NAME] = {};
		wmemcpy(var, tree->tree[index].func_params->params[i].variable, tree->tree[index].func_params->params[i].variable_length);
		wprintf(L"address %lu, %ls\n", frames->vars[frames->vars_amount - 1].address, var);*/
		(*ram)++;
	}
}

long long control_block_linkers(Compiler* cmp)
{
	long long index = cmp->index;
	
	long long root = cmp->index;
	Node* tree = cmp->code_tree->tree;
	char type = FUNCTION_FRAME;
	if (tree[root].key == KEY_WHILE)
		type = CYCLE_FRAME;
	else if (tree[root].key == KEY_IF)
		type = CONDITION_FRAME;

	Frame*     new_one = frame_new(MIN_FRAME_SIZE, type);
	Structure* current = cmp->scopes->stack;

	long long current_ram_index = 0;
	if (current->length - 1 != 0)
	{
		Frame* frame = current->buffer[current->length - 1];
		if (frame->vars_amount)
			current_ram_index = frame->vars[frame->vars_amount - 1].address + 1;
	} 
	
	make_new_frame(cmp->code_tree, root, new_one, root, current_ram_index);

	stack_push(&cmp->scopes, new_one);
	cmp->index = tree_left_son(cmp->code_tree, cmp->index);
	cmp->index = tree_right_son(cmp->code_tree, cmp->index);

	while (!tree_if_lief(cmp->code_tree, cmp->index))
	{
		cmp->index = current_line_code(cmp);
		if (cmp->index == ABSENT)
			break;
	}
	
	stack_pop(&cmp->scopes, &new_one);

	if (root == ABSENT)
		return ABSENT;

	return tree_right_son(cmp->code_tree, index);
}

void make_new_frame(Diff_tree* tree, long long root, Frame *cur_frame, long long curr_index, long long ram)
{
	if (tree_if_lief(tree, curr_index))
		return;

	if (tree->tree[tree_left_son(tree, curr_index)].type == FUNC)
	{
		/*wprintf(L"хата ");
		print_var(tree, tree_left_son(tree, curr_index));
		print_params(tree, tree_left_son(tree, curr_index));*/
		insert_params(cur_frame, tree, tree_left_son(tree, curr_index), &ram);
	}
	else if (tree->tree[tree_left_son(tree, curr_index)].type == KEY_WORD 
		  && tree->tree[tree_left_son(tree, curr_index)].key == KEY_DECL)
	{
		/*wprintf(L"хай буде ");
		print_var(tree, tree_right_son(tree, tree_left_son(tree, curr_index)));*/
		long long variable_index = tree_right_son(tree, tree_left_son(tree, curr_index));
		frame_insert(cur_frame, tree_get_variable(tree, variable_index), tree_get_variable_length(tree, variable_index), 0, ram);
		
		/*wchar_t var[MAX_VAR_NAME] = {};
		wmemcpy(var, tree->tree[tree_right_son(tree, tree_left_son(tree, curr_index))].variable, tree->tree[tree_right_son(tree, tree_left_son(tree, curr_index))].variable_length);
		wprintf(L"address %lu, %ls\n", cur_frame->vars[cur_frame->vars_amount - 1].address, var);
		*/
		ram++;
	}

	if (curr_index == root)
		curr_index = tree_left_son(tree, curr_index);

	make_new_frame(tree, root, cur_frame, tree_right_son(tree, curr_index), ram);
}

Var* get_variable_structure(Diff_tree* tree, long long index, Stack* cur_frame)
{
	Structure* current_node = cur_frame->stack;
	Frame*    current_frame = current_node->buffer[current_node->length - 1];

	size_t var_index       = ABSENT;
	size_t current_finding = ABSENT;

	size_t stack_scope = 1;

	wchar_t* var = NULL;
	size_t var_len = 0;
	for (; stack_scope < current_node->length; stack_scope++)
	{
		var     = tree_get_variable(tree, index);
		var_len = tree_get_variable_length(tree, index);

		current_finding = find_this_var(current_node->buffer[stack_scope], var, var_len);
		if (current_finding != ABSENT)
		{
			var_index = current_finding;
			break;
		}
	}
	if (var_index == ABSENT)
	{
		printf("Go home buddy\n");
		return NULL;
	}
	
	//Возвращает конкретную переменную фрейма
	return &(current_node->buffer[stack_scope]->vars[var_index]);
}


void print_variable(Compiler* cmp, long long address)
{
	fprintf(cmp->asm_code, "PUSH %u\n", address);
	fprintf(cmp->asm_code, "PUSH RDX\n");
	fprintf(cmp->asm_code, "ADD\n");
	fprintf(cmp->asm_code, "POP RCX\n");
	fprintf(cmp->asm_code, "POP [RCX]\n");
}
//Тут код возврата
int write_in_variable(Compiler *cmp, long long index)
{
	long long addrss = get_variable_structure(cmp->code_tree, index, cmp->scopes)->address;
	if (addrss == ABSENT)
	{
		fprintf(cmp->asm_code, "No variable found\n");

		return ABSENT;
	}
	print_variable(cmp, addrss);

	reduce_unused_values_amount(cmp);

	return 0;
}

//Тут код возврата
int read_from_variable(Compiler *cmp, long long index)
{
	long long addrss = get_variable_structure(cmp->code_tree, index, cmp->scopes)->address;
	if (addrss == ABSENT)
	{
		fprintf(cmp->asm_code, "No variable found\n");

		return ABSENT;
	}
	fprintf(cmp->asm_code, "PUSH %lld\n", addrss);
	fprintf(cmp->asm_code, "PUSH RDX\n");
	fprintf(cmp->asm_code, "ADD\n");
	fprintf(cmp->asm_code, "POP RCX\n");
	fprintf(cmp->asm_code, "PUSH [RCX]\n");

	cmp->unused_values_amount++;

	return 0;
}

const char JUMP_LABEL[] = "jump_label_";

void print_comparison(Compiler* cmp, comparison CMP)
{
	switch (CMP)
	{
	case COMP_MORE:
		fprintf(cmp->asm_code, "JA ");
		break;
	case COMP_LESS:
		fprintf(cmp->asm_code, "JB ");
		break;
	case COMP_EQUAL:
		fprintf(cmp->asm_code, "JE ");
		break;
	case COMP_NOT_EQUAL:
		fprintf(cmp->asm_code, "JNE ");
		break;
	default:
		break;
	}
	
	fprintf(cmp->asm_code, "#%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 0\n");
	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "JMP #%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	cmp->number_of_conditional_jump--;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 1\n");
	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "JMP #%s%lld\n\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	cmp->number_of_conditional_jump++;

	reduce_unused_values_amount(cmp);
}

const char* operations_texts[] =
{
	"",
	"",
	"ADD\n",
	"SUB\n",
	"MUL\n",
	"DIV\n",
	"",
	"SIN\n",
	"COS\n",
	"LN",
	"SQRT\n"
};

void print_operation(Compiler* cmp, operations OP)
{
	switch (OP)
	{
	/*case OP_PLUS:
		fprintf(cmp->asm_code, "ADD\n");
		reduce_unused_values_amount(cmp);
		break;
	case OP_SUB:
		fprintf(cmp->asm_code, "SUB\n");
		reduce_unused_values_amount(cmp);
		break;
	case OP_MUL:
		fprintf(cmp->asm_code, "MUL\n");
		reduce_unused_values_amount(cmp);
		break;
	case OP_DIV:
		fprintf(cmp->asm_code, "DIV\n");
		reduce_unused_values_amount(cmp);
		break;*/
	case OP_AND:
		print_and(cmp);
		break;
	case OP_OR:
		print_or(cmp);
		break;
	default:
		fprintf(cmp->asm_code, operations_texts[OP]);
		
		if (!(OP > OP_POW && OP < OP_AND))
			reduce_unused_values_amount(cmp);
		break;
	}
}

void print_or(Compiler* cmp)
{
	fprintf(cmp->asm_code, "PUSH 1\n");
	fprintf(cmp->asm_code, "JE ");
	fprintf(cmp->asm_code, "#%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump); //ушли или нет
	fprintf(cmp->asm_code, "PUSH 1\n");
	cmp->number_of_conditional_jump++;

	fprintf(cmp->asm_code, "JE ");
	fprintf(cmp->asm_code, "#%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 0\n");
	cmp->number_of_conditional_jump++;
	
	fprintf(cmp->asm_code, "JMP #%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	cmp->number_of_conditional_jump -= 2;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "POP\n");

	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 1\n");
	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);

	cmp->number_of_conditional_jump++;
	reduce_unused_values_amount(cmp);
}

void print_and(Compiler* cmp)
{
	fprintf(cmp->asm_code, "PUSH 0\n");
	fprintf(cmp->asm_code, "JE ");
	fprintf(cmp->asm_code, "#%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump); //ушли или нет
	fprintf(cmp->asm_code, "PUSH 0\n");
	cmp->number_of_conditional_jump++;

	fprintf(cmp->asm_code, "JE ");
	fprintf(cmp->asm_code, "#%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 1\n");
	cmp->number_of_conditional_jump++;

	fprintf(cmp->asm_code, "JMP #%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	cmp->number_of_conditional_jump -= 2;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "POP\n");

	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	fprintf(cmp->asm_code, "PUSH 0\n");
	cmp->number_of_conditional_jump++;
	fprintf(cmp->asm_code, "%s%lld:\n", JUMP_LABEL, cmp->number_of_conditional_jump);

	cmp->number_of_conditional_jump++;
	reduce_unused_values_amount(cmp);
}

long long print_jump_for_if(Compiler* cmp, long long index)
{
	if (compile_expression(cmp, tree_left_son(cmp->code_tree, index)) == ABSENT)
		return ABSENT;

	long long continue_number = cmp->number_of_conditional_jump;
	fprintf(cmp->asm_code, "PUSH 0\n");
	fprintf(cmp->asm_code, "JE #%s%lld\n", JUMP_LABEL, cmp->number_of_conditional_jump);
	long long last_jmp = cmp->number_of_conditional_jump;

	cmp->number_of_conditional_jump++;

	cmp->index = control_block_linkers(cmp);
	fprintf(cmp->asm_code, "JMP #%s%lld\n", JUMP_LABEL, last_jmp);

	reduce_unused_values_amount(cmp);

	fprintf(cmp->asm_code, "\n%s%lld:\n", JUMP_LABEL, continue_number);
	
	return cmp->index;
}

long long print_jump_for_while(Compiler* cmp, long long index)
{
	long long last_jump = cmp->number_of_conditional_jump;
	fprintf(cmp->asm_code, "\n%s%lld:\n", JUMP_LABEL, last_jump);

	cmp->number_of_conditional_jump++;
	if (compile_expression(cmp, tree_left_son(cmp->code_tree, index)) == ABSENT)
		return ABSENT;

	long long next_jump = cmp->number_of_conditional_jump;
	
	long long continue_number = cmp->number_of_conditional_jump;
	fprintf(cmp->asm_code, "PUSH 0\n");
	fprintf(cmp->asm_code, "JE #%s%lld\n", JUMP_LABEL, next_jump);

	cmp->number_of_conditional_jump++;

	cmp->index = control_block_linkers(cmp);
	fprintf(cmp->asm_code, "JMP #%s%lld\n", JUMP_LABEL, last_jump);

	reduce_unused_values_amount(cmp);

	fprintf(cmp->asm_code, "\n%s%lld:\n", JUMP_LABEL, next_jump);

	return cmp->index;
}

void move_rdx(Compiler* cmp, long long offset)
{
	fprintf(cmp->asm_code, "PUSH %lld\n", offset);
	fprintf(cmp->asm_code, "PUSH RDX\n");
	fprintf(cmp->asm_code, "ADD\n");
	fprintf(cmp->asm_code, "POP RDX\n");
}

void write_all_params(Compiler* cmp, long long index, Var *func)
{
	Frame* current_frame = NULL;
	stack_back(&cmp->scopes, &current_frame);

	long long current_offset = current_frame->vars_amount;

	long long params_index  = tree_left_son(cmp->code_tree, index);
	long long params_amount = func->args_amount;

	Param*    parameters   = cmp->code_tree->tree[params_index].func_params;
	long long input_params = func->args_amount;

	Frame* last = cmp->scopes->stack->buffer[cmp->scopes->stack->length - 1];
	size_t last_index = last->vars[last->vars_amount - 1].address + 1;

	/*fprintf(cmp->asm_code, "PUSH %lld\n", current_offset);
	fprintf(cmp->asm_code, "PUSH RDX\n");
	fprintf(cmp->asm_code, "ADD\n");
	fprintf(cmp->asm_code, "POP RDX\n");*/
	//move_rdx(cmp, current_offset);

	size_t counter = 0;

	long long linker_index = tree_right_son(cmp->code_tree, index);

	printf("param amount %lld last index %d linker index %lld cmp index %lld\n", params_amount, last_index, linker_index, cmp->index);
	while (!tree_if_lief(cmp->code_tree, linker_index) && counter < params_amount)
	{
		//move_rdx(cmp, -1 * current_offset);
		compile_expression(cmp, tree_left_son(cmp->code_tree, linker_index));
		//move_rdx(cmp, current_offset);

		print_variable(cmp, last_index);
		last_index++;

		linker_index = tree_right_son(cmp->code_tree, linker_index);

		counter++;
	}
	
	for (size_t i = counter; i < params_amount; i++)
	{
		fprintf(cmp->asm_code, "PUSH 0\n");
		fprintf(cmp->asm_code, "POP [%lld]\n", last_index);
		last_index++;
	}

	long long addrss = func->address;
	wchar_t* var = func->variable;
	size_t var_len = func->variable_length;

	move_rdx(cmp, current_offset);

	fprintf(cmp->asm_code, "CALL #");
	print_russian_name(cmp->asm_code, var, var_len);
	fprintf(cmp->asm_code, "\n");

	/*fprintf(cmp->asm_code, "PUSH %lld\n", current_offset);
	fprintf(cmp->asm_code, "PUSH RDX\n");
	fprintf(cmp->asm_code, "SUB\n");
	fprintf(cmp->asm_code, "POP RDX\n", current_offset);*/
	move_rdx(cmp,  -1* current_offset);
}

int check_unary_operation(Compiler* cmp, long long index)
{
	if (tree_get_type(cmp->code_tree, index) == OPERATION)
	{
		operations op = tree_get_operation(cmp->code_tree, index);

		if (op > OP_POW && op < OP_AND)
			return 1;
	}

	return 0;
}

int compile_expression(Compiler *cmp, long long index)
{
	printf("compiling %lld\n", index);
	if (tree_if_lief(cmp->code_tree, index))
	{
		char type = tree_get_type(cmp->code_tree, index);
		if (type == VARIABLE)
		{
			int returning = read_from_variable(cmp, index);
			if (returning == ABSENT)
				return ABSENT;
		}
		else if (type == NUMBER)
		{
			fprintf(cmp->asm_code, "PUSH %lg\n", tree_get_number(cmp->code_tree, index));
			cmp->unused_values_amount++;
		}
		else if (type == KEY_WORD)
		{
			fprintf(cmp->asm_code, "IN\n");
			cmp->unused_values_amount++;
		}
		
		return 0;
	}
	else if (tree_get_type(cmp->code_tree, index) == FUNC)
	{
		Var* var_index = get_variable_structure(cmp->code_tree, index, cmp->scopes);
		write_all_params(cmp, index, var_index);

		return 0;
	}
	
	if (compile_expression(cmp, tree_right_son(cmp->code_tree, index)) == ABSENT)
		return ABSENT;
	if (!check_unary_operation(cmp, index))
	{
		if (compile_expression(cmp, tree_left_son(cmp->code_tree, index)) == ABSENT)
			return ABSENT;
	}

	char type = tree_get_type(cmp->code_tree, index);

	if (type == OPERATION)
		print_operation(cmp, tree_get_operation(cmp->code_tree, index));
	else if (type == COMPARISON)
		print_comparison(cmp, tree_get_comp(cmp->code_tree, index));

	return 0;
}

void reduce_unused_values_amount(Compiler *cmp)
{
	assert(cmp->unused_values_amount > 0);

	cmp->unused_values_amount--;
}

long long current_line_code(Compiler *cmp)
{
	long long linker_index = cmp->index;
	long long index = tree_left_son(cmp->code_tree, linker_index);

	long long unused_vars_before = cmp->unused_values_amount;

	long long last_jump = 0;
	long long next_jump = 0;
	char type = tree_get_type(cmp->code_tree, index);
	if (type == KEY_WORD)
	{
		key_word current_key = tree_get_key(cmp->code_tree, index);
		switch (current_key)
		{
		case KEY_ASSIGN:
			if (compile_expression(cmp, tree_right_son(cmp->code_tree, index)) == ABSENT)
				return ABSENT;
			if (write_in_variable(cmp, tree_left_son(cmp->code_tree, index)) == ABSENT)
				return ABSENT;
			break;
		case KEY_PRINT:
			if (compile_expression(cmp, tree_right_son(cmp->code_tree, index)) == ABSENT)
				return ABSENT;
			fprintf(cmp->asm_code, "OUT\n");
			reduce_unused_values_amount(cmp);
			break;
		case KEY_RETURN:
			if (compile_expression(cmp, tree_right_son(cmp->code_tree, index)) == ABSENT)
				return ABSENT;
			fprintf(cmp->asm_code, "REV\n");
			break;
		case KEY_IF:
			if (print_jump_for_if(cmp, index) == ABSENT)
				return ABSENT;
			break;
		case KEY_WHILE:
			if (print_jump_for_while(cmp, index) == ABSENT)
				return ABSENT;
			break;
		/*case KEY_SCAN:
			fprintf(cmp->asm_code, "IN\n");
			break;*/
		default:
			break;
		}
	}
	else
		if (compile_expression(cmp, index) == ABSENT)
			return ABSENT;

	while (cmp->unused_values_amount > unused_vars_before)
	{
		fprintf(cmp->asm_code, "POP\n");
		reduce_unused_values_amount(cmp);
	}
	
	return tree_right_son(cmp->code_tree, linker_index);
}

void print_russian_name(FILE* file, wchar_t* pointer, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		fprintf(file, "%04x", *pointer);
		pointer++;
	}
}

void print_const_russian_name(FILE* file, const wchar_t* pointer)
{
	size_t length = wcslen(pointer);
	for (size_t i = 0; i < length; i++)
	{
		fprintf(file, "%04x", *pointer);
		pointer++;
	}
}

int code_generation(Diff_tree* tree)
{
	Compiler *new_code = compiler_new(tree);

	long long current_ram_index = 0;
	size_t stack_size = new_code->scopes->stack->length;
	Frame** frames_buffer = new_code->scopes->stack->buffer;
	if (stack_size - 1 != 0)
	{
		Frame* current_frame = frames_buffer[stack_size - 1];
		if (current_frame->vars_amount)
			current_ram_index = current_frame->vars[current_frame->vars_amount - 1].address + 1;
	}

	long long current_tree_index = tree->root_index;
	while (!tree_if_lief(tree, new_code->index))
	{
		long long var_index = tree_left_son(tree, new_code->index);
		wchar_t* var = tree_get_variable(tree, var_index);
		size_t var_len = tree_get_variable_length(tree, var_index);
		print_russian_name(new_code->asm_code, var, var_len);
		fprintf(new_code->asm_code, ":\n");
		
		new_code->index = control_block_linkers(new_code);
		if (new_code->index == ABSENT)
			break;
		fprintf(new_code->asm_code, "REV\n");
	}
	int returning = 0;
	if (new_code->index == ABSENT)
		returning = 1;

	compiler_delete(new_code);

	return returning;
}
