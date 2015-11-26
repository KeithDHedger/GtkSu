
include ../flagsandlibs

gtksuwrap_SOURCES = suwrap.cpp

bin_PROGRAMS = gtksuwrap

install-exec-hook:
	sudo chown root:root $(DESTDIR)${prefix}/bin/gtksuwrap
	sudo chmod ug+s $(DESTDIR)${prefix}/bin/gtksuwrap
