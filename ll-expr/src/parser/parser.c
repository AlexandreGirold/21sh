#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

static struct ast *parse_exp_internal(enum parser_status *status,
                                      struct lexer *lexer);
static struct ast *parse_sexp_internal(enum parser_status *status,
                                       struct lexer *lexer);
static struct ast *parse_texp_internal(enum parser_status *status,
                                       struct lexer *lexer);
static struct ast *parse_number_or_paren(enum parser_status *status,
                                         struct lexer *lexer);
static struct ast *parse_negative(enum parser_status *status,
                                  struct lexer *lexer);

static struct ast *parse_exp_internal(enum parser_status *status,
                                      struct lexer *lexer);
static struct ast *parse_negative_number(enum parser_status *status,
                                         struct lexer *lexer);
static struct ast *parse_negative_paren(enum parser_status *status,
                                        struct lexer *lexer);

struct ast *parse(enum parser_status *status, struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type == TOKEN_EOF)
    {
        *status = PARSER_OK;
        return NULL;
    }

    struct ast *result = parse_exp_internal(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
        return NULL;
    }

    tok = lexer_peek(lexer);
    if (tok.type != TOKEN_EOF)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        ast_free(result);
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
        return NULL;
    }

    *status = PARSER_OK;
    return result;
}

static struct ast *parse_exp_internal(enum parser_status *status,
                                      struct lexer *lexer)
{
    struct ast *left = parse_sexp_internal(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
        return NULL;
    }
    struct token tok = lexer_peek(lexer);
    while (tok.type == TOKEN_PLUS || tok.type == TOKEN_MINUS)
    {
        lexer_pop(lexer);
        struct ast *right = parse_sexp_internal(status, lexer);
        if (*status != PARSER_OK)
        {
            ast_free(left);
            fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
            return NULL;
        }

        struct ast *new_node =
            ast_new(tok.type == TOKEN_PLUS ? AST_PLUS : AST_MINUS);
        if (!new_node)
        {
            ast_free(left);
            ast_free(right);
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
            return NULL;
        }

        new_node->left = left;
        new_node->right = right;
        left = new_node;

        tok = lexer_peek(lexer);
    }

    *status = PARSER_OK;
    return left;
}

static struct ast *parse_sexp_internal(enum parser_status *status,
                                       struct lexer *lexer)
{
    struct ast *left = parse_texp_internal(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
        return NULL;
    }

    struct token tok = lexer_peek(lexer);
    while (tok.type == TOKEN_MUL || tok.type == TOKEN_DIV)
    {
        lexer_pop(lexer);
        struct ast *right = parse_texp_internal(status, lexer);
        if (*status != PARSER_OK)
        {
            ast_free(left);
            fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
            return NULL;
        }

        struct ast *new_node =
            ast_new(tok.type == TOKEN_MUL ? AST_MUL : AST_DIV);
        if (!new_node)
        {
            ast_free(left);
            ast_free(right);
            *status = PARSER_UNEXPECTED_TOKEN;
            fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
            return NULL;
        }

        new_node->left = left;
        new_node->right = right;
        left = new_node;

        tok = lexer_peek(lexer);
    }

    *status = PARSER_OK;
    return left;
}

static struct ast *parse_texp_internal(enum parser_status *status,
                                       struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_NUMBER || tok.type == TOKEN_LEFT_PAR)
        return parse_number_or_paren(status, lexer);

    if (tok.type == TOKEN_MINUS)
        return parse_negative(status, lexer);

    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
    return NULL;
}

static struct ast *parse_number_or_paren(enum parser_status *status,
                                         struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_NUMBER)
    {
        lexer_pop(lexer);
        struct ast *node = ast_new(AST_NUMBER);
        if (!node)
        {
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }
        node->value = tok.value;
        *status = PARSER_OK;
        return node;
    }

    if (tok.type == TOKEN_LEFT_PAR)
    {
        lexer_pop(lexer);
        struct ast *exp = parse_exp_internal(status, lexer);
        if (*status != PARSER_OK)
        {
            fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
            return NULL;
        }

        tok = lexer_peek(lexer);
        if (tok.type != TOKEN_RIGHT_PAR)
        {
            ast_free(exp);
            *status = PARSER_UNEXPECTED_TOKEN;
            return NULL;
        }

        lexer_pop(lexer);
        *status = PARSER_OK;
        return exp;
    }

    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "PARSER_UNEXPECTED_TOKEN");
    return NULL;
}

static struct ast *parse_negative(enum parser_status *status,
                                  struct lexer *lexer)
{
    lexer_pop(lexer);
    struct token tok = lexer_peek(lexer);

    if (tok.type == TOKEN_NUMBER)
    {
        return parse_negative_number(status, lexer);
    }

    if (tok.type == TOKEN_LEFT_PAR)
    {
        return parse_negative_paren(status, lexer);
    }

    *status = PARSER_UNEXPECTED_TOKEN;
    fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
    return NULL;
}

static struct ast *parse_negative_number(enum parser_status *status,
                                         struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    lexer_pop(lexer);
    struct ast *node = ast_new(AST_NEG);
    if (!node)
    {
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
        return NULL;
    }

    struct ast *left = ast_new(AST_NUMBER);
    if (!left)
    {
        free(node);
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
        return NULL;
    }

    left->value = tok.value;
    node->left = left;
    *status = PARSER_OK;
    return node;
}

static struct ast *parse_negative_paren(enum parser_status *status,
                                        struct lexer *lexer)
{
    struct ast *exp = parse_exp_internal(status, lexer);
    if (*status != PARSER_OK)
    {
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
        return NULL;
    }

    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_RIGHT_PAR)
    {
        ast_free(exp);
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
        return NULL;
    }

    lexer_pop(lexer);
    struct ast *node = ast_new(AST_NEG);
    if (!node)
    {
        ast_free(exp);
        *status = PARSER_UNEXPECTED_TOKEN;
        fprintf(stderr, "PARSER_UNEXPECTED_TOKEN\n");
        return NULL;
    }

    node->left = exp;
    *status = PARSER_OK;
    return node;
}
