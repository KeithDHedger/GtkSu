#
#
AC_INIT([GtkSu],[0.1.0],[PROJ],[GtkSu])

AC_CONFIG_HEADER([config.h])
AC_PROG_CXX

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_HEADER_STDC

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AM_PROG_CC_C_O

CXXFLAGS=-Wall
AC_SUBST(CXXFLAGS)

CFLAGS=-Wall
AC_SUBST(CFLAGS)
PKG_PROG_PKG_CONFIG

AC_ARG_ENABLE(qt5,[  --enable-qt5		Enable QT5.],[enable_qt5=$enableval],[enable_qt5="no"])
if test $enable_qt5 = "yes" ;then
#	QT_MODULES="Qt5Gui >= 5.2.1"
#	PKG_CHECK_MODULES(QT, $QT_MODULES)
#	AC_SUBST(QT_CFLAGS)
#	AC_SUBST(QT_LIBS)

	QTWIDGETS_MODULES="Qt5Widgets >= 5.2.1"
	PKG_CHECK_MODULES(QTWIDGETS, $QTWIDGETS_MODULES)
	AC_SUBST(QTWIDGETS_CFLAGS)
	AC_SUBST(QTWIDGETS_LIBS)

	AC_DEFINE([_USEQT5_], [], [Build Qt5])
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

AC_OUTPUT(Makefile GtkSu/app/Makefile GtkSu/MakeSuWrap/Makefile)

