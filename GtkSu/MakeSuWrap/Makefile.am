
include ../flagsandlibs

gtksuwrap_SOURCES = suwrap.cpp

gtksuwrap_CFLAGS = -O3 -export-dynamic
gtksuwrap_CXXFLAGS = -O3 -export-dynamic

bin_PROGRAMS = gtksuwrap

#all-local:
#	sudo chown root:root gtksuwrap
#	sudo chmod ug+s gtksuwrap

install-exec-hook:
	sudo chown root:root $(DESTDIR)${prefix}/bin/gtksuwrap
	sudo chmod ug+s $(DESTDIR)${prefix}/bin/gtksuwrap
