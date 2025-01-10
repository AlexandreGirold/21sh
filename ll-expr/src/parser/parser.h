#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
};

/**
 * \brief Parses an expression or nothing.
 *
 * input =     EOF
 *          |  exp EOF ;
 */
struct ast *parse(enum parser_status *status, struct lexer *lexer);

/**
 * \brief Parses sexp expressions separated by + and -.
 *
 * exp =       sexp  { ( '+' | '-' ) sexp } ;
 */
struct ast *parse_exp(enum parser_status *status, struct lexer *lexer);

/**
 * \brief Parses texp expressions separated by * and /.
 *
 * sexp =      texp  { ('*' | '/' ) texp } ;
 */
struct ast *parse_sexp(enum parser_status *status, struct lexer *lexer);

/**
 * \brief Parses a number, a - a number, or a parenthesized expression.
 *
 * texp =      NUMBER
 *          |  '-' NUMBER
 *          |  '-' '(' exp ')'
 *          |  '(' exp ')'
 *          ;
 */
struct ast *parse_texp(enum parser_status *status, struct lexer *lexer);

#endif /* !PARSER_H */
