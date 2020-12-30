#pragma once

const size_t MIN_PARAM = 1;

const int  MAX_MESSAGE_LEN = 255;
const int  MAX_PICTURE_NAME = 255;

const int MAX_ANSWER_LEN = 4;

const char PICTURE_NAME[] = "code_tree_picture";
const char PICTURE_EXPANSION[] = "png";
const char PICTURE_CODE_EXPANSION[] = "txt";

const char ASM_FILE_NAME[] = "asm.xax";

const char COLOR[] = "#FEC9E3";
const char COLOR_OTHER[] = "HotPink";

//const int ABSENT = -1;

const int MAX_VAR_NAME = 255;

const int MAX_OP_LEN = 50;

const int PLUS_LEN = 5;
const int MINUS_LEN = 7;
const int SIN_LEN = 3;
const int COS_LEN = 3;
const int LN_LEN = 2;
const int SQRT_LEN = 4;
const int INC_LEN = 6;
const int SIMPLE_LEN = 1;

const int MAX_KEY_LEN = 50;

const int FUNC_LEN = 4;
const int SEP_LEN = 3;
const int PRIORITY_LEN = 1;

const int WHILE_LEN = 7;
const int WHILE_OFFSET_LEN = 5;

const int IF_LEN = 4;
const int DECL_LEN = 4;

const int RETURN_LEN = 11;
const int RETURN_OFFSET_LEN = 6;

const int PRINT_LEN = 7;

const int SCAN_OFFSET = 5;
const int SCAN_LEN = 9;

const int PARAM_LEN = 1;
const int END_PARAM_LEN = 9;
const int LINE_COMMENT_LEN = 2;

const int BLOCK_COMMENT_BEGIN_LEN = 10;
const int BLOCK_COM_BEGIN_FIRST_OFFSET = 3;
const int BLOCK_COM_BEGIN_SECOND_OFFSET = 6;

const int BLOCK_COMMENT_END_LEN = 10;
const int BLOCK_COM_END_FIRST_OFFSET = 2;
const int BLOCK_COM_END_SECOND_OFFSET = 6;

const int LEFT_BRACKET_LEN = 3;

const int RIGHT_BRACKET_LEN = 6;
const int RIGHT_BRACKET_OFFSET = 2;

const int KEY_CALLING_LEN = 7;

const int OR_LEN = 3;
const int AND_LEN = 1;

const int ASSIGN_LEN = 2;

const char NUMBER = 0;

const char VARIABLE = 1;

const char OPERATION = 2;

typedef enum operations_names
{
    OP_INC,
    OP_DEC,
    OP_PLUS,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,
    OP_SIN,
    OP_COS,
    OP_LN,
    OP_SQRT,
    OP_AND,
    OP_OR,
    NO_OP
}operations;

const char COMPARISON = 3;

typedef enum comparison_names
{
    COMP_MORE,
    COMP_LESS,
    COMP_EQUAL,
    COMP_NOT_EQUAL,
    NO_COMP
}comparison;

const char KEY_WORD = 4;

typedef enum key_words_names
{
    KEY_FUNCTION,
    KEY_SEPARATOR,
    KEY_OPEN_PRIORITY,
    KEY_CLOSE_PRIORITY,
    KEY_WHILE,
    KEY_IF,
    KEY_DECL,
    KEY_ASSIGN,
    KEY_RETURN,
    KEY_PRINT,
    KEY_SCAN,
    KEY_PARAM,
    KEY_END_PARAM,
    KEY_LINE_COMMENT,
    KEY_BLOCK_COMMENT_BEGIN,
    KEY_BLOCK_COMMENT_END,
    KEY_LEFT_BRACKET,
    KEY_RIGHT_BRACKET,
    KEY_CALLING,
    NO_KEY
}key_word;

const char FUNC = 5;

const char LINKER = 6;

const char PARAM = 7;

const char NOTHING = 8;

const int ONE_STEP = 1;
const int TWO_STEPS = 2;
const int THREE_STEPS = 3;

typedef enum Compilation_errors
{
    LEXICAL_ERROR,
    PARSER_ERROR,
    COMPILATION_ERROR
}Compilation_errors;


