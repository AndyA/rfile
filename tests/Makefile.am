noinst_PROGRAMS=basic
basic_SOURCES=basic.c tap.c
basic_LDADD=../src/librfile.la

check: all
	$(PROVE) -e '' ./basic
