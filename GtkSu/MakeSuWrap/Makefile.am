
include ../flagsandlibs

gtksuwrap_SOURCES = suwrap.cpp

gtksuwrap_CFLAGS = -O3 -export-dynamic
gtksuwrap_CXXFLAGS = -O3 -export-dynamic

bin_PROGRAMS = gtksuwrap

all-local:
	sudo chown root:root gtksuwrap
	sudo chmod ug+s gtksuwrap

install-exec-hook:
	sudo chown root:root $(DESTDIR)${prefix}/bin/gtksuwrap
	sudo chmod ug+s $(DESTDIR)${prefix}/bin/gtksuwrap

#	cp ../resources/pixmaps/GtkSu.desktop /usr/share/applications/GtkSu.desktop
#	echo Icon=$(DESTDIR)${prefix}/share/GtkSu/pixmaps/GtkSu.png >> /usr/share/applications/GtkSu.desktop
#	echo Exec=$(DESTDIR)${prefix}/bin/gtksu >> /usr/share/applications/GtkSu.desktop


#uninstall-hook:
#	rm -f /usr/share/applications/GtkSu.desktop
##	update-menus
#	rm -fr $(pkgdatadir)

