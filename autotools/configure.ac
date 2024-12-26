AC_INIT([42sh], [1.0], [alexandre.girold@epita.fr])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
#AC_CONFIG_SRCDIR([autotools/src/42sh.c])

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR([act-if-fail])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([\
                Makefile \
                src/Makefile \
                src/ast/Makefile \
                src/lexer/Makefile \
                src/parser/Makefile
                 ])
AC_OUTPUT
