EXTRA_DIST=README bootstrap.sh
SUBDIRS=src tests
dist_doc_DATA=README

.PHONY: test check

test: check

check: all
	$(PROVE) -e '' ./tests/basic
