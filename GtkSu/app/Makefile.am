
AUTOMAKE_OPTIONS = subdir-objects

include ../flagsandlibs
include ../sources

gtksu_SOURCES = $(COMMONSRC)

bin_PROGRAMS = gtksu

man1_MANS = ../resources/man/gtksu.1.gz
  
if KTSUSSLINK
ktsuss = "makelink"
endif
     
install-exec-hook:
	if [ x${ktsuss} = xmakelink ];then ln -sfv gtksu $(DESTDIR)${prefix}/bin/ktsuss;fi


