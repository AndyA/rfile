AC_INIT([rfile],[m4_esyscmd(sh tools/get-version.sh)])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])

BT_PROG_XCODE
AC_PROG_CC([clang gcc c99 cc])
AC_PATH_PROG([PERL], [perl])
AC_PATH_PROG([PROVE], [prove])

LT_INIT

AC_PROG_CC_C99
AC_SYS_LARGEFILE
BT_PROG_CC_WARN

AC_CONFIG_FILES([
Makefile
t/Makefile
m4/Makefile
])

AC_OUTPUT
