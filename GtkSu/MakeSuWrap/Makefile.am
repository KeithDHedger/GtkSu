
include ../flagsandlibs

gtksuwrap_SOURCES = suwrap.cpp

bin_PROGRAMS = gtksuwrap

install-exec-hook:
	chown root:root $(DESTDIR)${prefix}/bin/gtksuwrap||true
	chmod ug+s $(DESTDIR)${prefix}/bin/gtksuwrap||true
