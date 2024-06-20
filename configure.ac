#
#
AC_INIT([GtkSu],[0.1.6],[PROJ],[GtkSu])

#AC_CONFIG_HEADER([config.h])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
#AC_HEADER_STDC

PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE(qt6,[  --enable-gtk		Enable GTK.],[enable_gtk=$enableval],[enable_gtk="no"])
if test $enable_gtk = "no" ;then
	QTWIDGETS_MODULES="Qt6Widgets >= 6.6.0"
	PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES, [got6=yes], [:])
	if test "$got6" = "yes"; then
		AC_MSG_NOTICE([Using Qt6])
		AC_DEFINE([_USEQT_], [], [Build Qt])
	else
		QTWIDGETS_MODULES="Qt5Widgets >= 5.15.0"
		PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES)
		AC_MSG_NOTICE([Using Qt5])
	fi

	AC_DEFINE([_USEQT_], [], [Build Qt])
	AC_SUBST(QTWIDGETS_CFLAGS)
	AC_SUBST(QTWIDGETS_LIBS)
else
	GTK_MODULES="gtk+-2.0 >= 2.24.0"
	PKG_CHECK_MODULES(GTK, $GTK_MODULES)
	AC_SUBST(GTK_CFLAGS)
	AC_SUBST(GTK_LIBS)
fi

GLIB_MODULES="glib-2.0 >= 2.34.1"
PKG_CHECK_MODULES(GLIB, $GLIB_MODULES)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_MSG_CHECKING(pretend to be ktsuss)

AC_ARG_ENABLE(ktsuss-link,
     [  --enable-ktsuss-link    Create a symlink from ktsuss to gtksu.],
     [case "${enableval}" in
       yes) ktsuss=true ;;
       no)  ktsuss=false ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-ktsuss-link ) ;;
     esac],[ktsuss=false])
     AM_CONDITIONAL(KTSUSSLINK, test x$ktsuss = xtrue)
AC_MSG_RESULT($ktsuss)

AC_CHECK_FUNC(clearenv,AC_DEFINE([_GOTCLEARENV_], [],["Clear environment"]))

#AC_OUTPUT(Makefile GtkSu/app/Makefile GtkSu/MakeSuWrap/Makefile)

AC_CONFIG_FILES(Makefile GtkSu/app/Makefile GtkSu/MakeSuWrap/Makefile)
AC_OUTPUT
