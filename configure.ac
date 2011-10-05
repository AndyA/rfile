AC_PREREQ([2.67])
AC_INIT([rfile], [0.0.1], [andy@hexten.net])
LT_INIT([disable-static])
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_CONFIG_SRCDIR([src/rfile.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_ARG_VAR([PROVE], [Test::Harness runner])

CFLAGS="-Wall -Werror"

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_SED
AC_PATH_PROG([PROVE], [prove])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
