ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src tests
EXTRA_DIST = autogen.sh

.PHONY: scratch
scratch:
	mkdir -p scratch/subdir
	touch scratch/file-a scratch/subdir/file-{a,b,c}
