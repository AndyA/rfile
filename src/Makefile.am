lib_LTLIBRARIES=librfile.la
librfile_la_SOURCES=rfile.c bits.c
librfile_la_LDFLAGS=-version-info 1:0:0
include_HEADERS=rfile.h

bin_PROGRAMS=rfcat
rfcat_SOURCES=rfcat.c
rfcat_LDADD=librfile.la

rfile_struct.c rfile_struct.h: rfile.h ../tools/serializator.pl
	$(PERL) ../tools/serializator.pl -c rfile_struct.c -h rfile_struct.h rfile.h

rfile.c: rfile_struct.c rfile_struct.h
