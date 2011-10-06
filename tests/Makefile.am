AUTOMAKE_OPTIONS=-Wno-portability
noinst_PROGRAMS=basic
noinst_LIBRARIES=libtest.a
basic_SOURCES=basic.c
basic_LDADD=libtest.a ../src/librfile.la
libtest_a_SOURCES=tap.c

check: all
	$(PROVE) -e '' $(addprefix ./, $(noinst_PROGRAMS))
