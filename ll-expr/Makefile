CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -pedantic -Wvla -fsanitize=address
CPPFLAGS = -Isrc -Isrc/parser -Isrc/lexer -Isrc/ast
LDFLAGS = -fsanitize=address

OBJS = src/ast/ast.o \
	src/lexer/lexer.o \
	src/parser/parser.o

OBJ_AST = src/eval/ast_print.o
OBJ_RPN = src/eval/rpn_print.o
OBJ_TOKEN = src/eval/token_printer.o

all: ast rpn token

ast: $(OBJS) $(OBJ_AST)
	$(CC) $(LDFLAGS) -o eval_ast $(OBJS) $(OBJ_AST)

rpn: $(OBJS) $(OBJ_RPN)
	$(CC) $(LDFLAGS) -o eval_rpn $(OBJS) $(OBJ_RPN)

token: $(OBJS) $(OBJ_TOKEN)
	$(CC) $(LDFLAGS) -o eval_token $(OBJS) $(OBJ_TOKEN)

clean:
	$(RM) $(OBJS) $(OBJ_AST) $(OBJ_TOKEN) $(OBJ_RPN) eval_*

