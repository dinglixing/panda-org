
#ifndef __LANG_LEX_INC__
#define __LANG_LEX_INC__

#include "config.h"

#include "heap.h"

enum LEX_TOKEN_TYPE{
    TOK_EOF = 0,
    TOK_ID  = 256,
    TOK_NUM,
    TOK_STR,

    TOK_EQ,                 // ==
    TOK_NE,                 // !=
    TOK_GE,                 // >=
    TOK_LE,                 // <=

    TOK_ADDASSIGN,          // +=
    TOK_SUBASSIGN,          // -=
    TOK_MULASSIGN,          // *=
    TOK_DIVASSIGN,          // /=
    TOK_MODASSIGN,          // %=
    TOK_ANDASSIGN,          // &=
    TOK_ORASSIGN,           // |=
    TOK_XORASSIGN,          // ^=
    TOK_NOTASSIGN,          // ~=
    TOK_LSHIFTASSIGN,       // <<=
    TOK_RSHIFTASSIGN,       // >>=

    TOK_LSHIFT,             // <<
    TOK_RSHIFT,             // >>

    TOK_LOGICAND,           // &&
    TOK_LOGICOR,            // ||

    /* Key words */
    TOK_UND,
    TOK_NAN,
    TOK_NULL,
    TOK_TRUE,
    TOK_FALSE,

    TOK_IN,
    TOK_IF,
    TOK_VAR,
    TOK_DEF,
    TOK_RET,
    TOK_ELSE,
    TOK_ELIF,
    TOK_WHILE,
    TOK_BREAK,
    TOK_CONTINUE
};

typedef struct lexer_t {
    int  curr_ch;
    int  next_ch;
    int  curr_tok;
    int  line, col;
    int  line_end, line_pos;

    int  token_buf_size;
    int  line_buf_size;
    int  token_len;

    heap_t heap;
    char *line_buf;
    char *(*line_more)(void);
    char token_buf[TOKEN_MAX_SIZE];
} lexer_t;

typedef struct token_t {
    int type;
    int line, col;
    int value; // value or length of id | string
    char *text;
} token_t;

int lex_init(lexer_t *lex, const char *input, char *(*more)(void));
int lex_deinit(lexer_t *lex);

int lex_token(lexer_t *lex, token_t *tok);
int lex_match(lexer_t *lex, int tok);
int lex_position(lexer_t *lex, int *line, int *col);

#endif /* __LANG_LEX_INC__ */

