TESTBIN = basic refs filename

TESTPERL = perl.t

noinst_PROGRAMS = wrap $(TESTBIN)

noinst_LTLIBRARIES = libtest-support.la

libtest_support_la_SOURCES = \
	tap.c tap.h testutil.c testutil.h

libtest_support_la_LDFLAGS = -static

LDADD = libtest-support.la ../librfile.la

test: $(TESTBIN)
	$(PROVE) -e ./wrap $(TESTBIN) $(TESTPERL)

valgrind: $(TESTBIN)
	$(PROVE) -e 'valgrind -q' $(addprefix ./,$(TESTBIN))
