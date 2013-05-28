DIST_SUBDIRS = t m4

EXTRA_DIST = jd_path.h jd_utf8.h

SUBDIRS = . t

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = librfile.la

include_HEADERS = rfile.h

bin_PROGRAMS = rfcat

librfile_la_SOURCES = \
	rfile.c rfile.h \
	rfile_bits.c rfile_bits.h \
	rfile_filename.c rfile_filename.h

librfile_la_LDFLAGS = -avoid-version

rfcat_SOURCES = rfcat.c
rfcat_LDADD = librfile.la

test: all
	cd t && $(MAKE) test

rfile_struct.c rfile_struct.h: rfile.h tools/serializator.pl
	$(PERL) tools/serializator.pl -c rfile_struct.c -h rfile_struct.h rfile.h

rfile.c: rfile_struct.c rfile_struct.h

clean-local:
	-rm -rf rfile_struct.c rfile_struct.h
