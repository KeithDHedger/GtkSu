
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
	echo "XXXXXXXX"

man1_MANS = ../resources/man/gtksu.1

install-exec-hook:
	cp ../resources/pixmaps/GtkSu.desktop /usr/share/applications/GtkSu.desktop
	echo Icon=${prefix}/share/GtkSu/pixmaps/GtkSu.png >> /usr/share/applications/GtkSu.desktop
	echo Exec=${prefix}/bin/gtksu >> /usr/share/applications/GtkSu.desktop


uninstall-hook:
	rm -f /usr/share/applications/GtkSu.desktop
#	update-menus
	rm -fr $(pkgdatadir)

