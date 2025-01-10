#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

static struct token get_next_token(struct lexer *lexer);

struct lexer *lexer_new(const char *input)
{
    struct lexer *lexer = malloc(sizeof(struct lexer));
    if (!lexer)
        return NULL;
    lexer->input = input;
    lexer->pos = 0;
    lexer->current_tok.type = TOKEN_EOF;
    lexer->current_tok.value = 0;
    return lexer;
}

void lexer_free(struct lexer *lexer)
{
    if (lexer)
        free(lexer);
}

struct token lexer_next_token(struct lexer *lexer)
{
    return get_next_token(lexer);
}

struct token lexer_peek(struct lexer *lexer)
{
    if (lexer->current_tok.type == TOKEN_EOF)
        lexer->current_tok = lexer_next_token(lexer);
    return lexer->current_tok;
}

struct token lexer_pop(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    lexer->current_tok.type = TOKEN_EOF;
    return tok;
}

static struct token create_token(enum token_type type, ssize_t value)
{
    return (struct token){ .type = type, .value = value };
}

static struct token handle_number(struct lexer *lexer)
{
    ssize_t value = 0;
    while (isdigit(lexer->input[lexer->pos]))
    {
        value = value * 10 + (lexer->input[lexer->pos] - '0');
        lexer->pos++;
    }
    return create_token(TOKEN_NUMBER, value);
}

static struct token get_next_token(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\0')
    {
        char current_char = lexer->input[lexer->pos];

        if (isspace(current_char))
        {
            lexer->pos++;
            continue;
        }

        switch (current_char)
        {
        case '+':
            lexer->pos++;
            return create_token(TOKEN_PLUS, 0);
        case '-':
            lexer->pos++;
            return create_token(TOKEN_MINUS, 0);
        case '*':
            lexer->pos++;
            return create_token(TOKEN_MUL, 0);
        case '/':
            lexer->pos++;
            return create_token(TOKEN_DIV, 0);
        case '(':
            lexer->pos++;
            return create_token(TOKEN_LEFT_PAR, 0);
        case ')':
            lexer->pos++;
            return create_token(TOKEN_RIGHT_PAR, 0);
        default:
            if (isdigit(current_char))
                return handle_number(lexer);
            fprintf(stderr,
                    "eval_token: lexer_next_token: token %c is not valid\n",
                    current_char);
            lexer->pos++;
            return create_token(TOKEN_ERROR, 0);
        }
    }
    return create_token(TOKEN_EOF, 0);
}
