

include ../flagsandlibs
include ../sources

gtksudebug_SOURCES = $(COMMONSRC)

gtksudebug_CFLAGS = -ggdb -O0 -export-dynamic
gtksudebug_CXXFLAGS = -ggdb -O0 -export-dynamic

bin_PROGRAMS = gtksudebug

install-exec-local:
	cd $(DESTDIR)$(bindir) && \
	rm gtksudebug

