
include ../flagsandlibs
include ../sources

gtksu_SOURCES = $(COMMONSRC)

gtksu_CFLAGS = -O3 -export-dynamic
gtksu_CXXFLAGS = -O3 -export-dynamic

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

scriptfilesdir = $(pkgdatadir)/scripts
scriptfiles_SCRIPTS = ../resources/scripts/README

bin_PROGRAMS = gtksu

man1_MANS = ../resources/man/gtksu.1

install-exec-hook:
	mkdir -vp $(DESTDIR)${prefix}/share/applications
	cp ../resources/pixmaps/GtkSu.desktop $(DESTDIR)${prefix}/share/applications/GtkSu.desktop
	echo Icon=$(DESTDIR)${prefix}/share/GtkSu/pixmaps/GtkSu.png >> $(DESTDIR)${prefix}/share/applications/GtkSu.desktop
	echo Exec=$(DESTDIR)${prefix}/bin/gtksu >> $(DESTDIR)${prefix}/share/applications/GtkSu.desktop
	ln -sv gtksu $(DESTDIR)${prefix}/bin/ktsuss

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/GtkSu.desktop
#	update-menus
	rm -fr $(pkgdatadir)

