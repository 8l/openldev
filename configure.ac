dnl Process this file with autoconf to produce a configure script.

AC_INIT(configure.ac)

AM_INIT_AUTOMAKE(OpenLDev, 1.0)
AM_CONFIG_HEADER(config.h)
AC_PREFIX_DEFAULT("/usr")

AC_PROG_CC
AC_PROG_CPP
AC_LANG_C
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_ISC_POSIX
AC_HEADER_STDC
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_PATH_PROG(PKGCONFIG_CONFIG, pkg-config,no)
if test x$PKGCONFIG_CONFIG = xno; then
  AC_MSG_ERROR(Please install the pkg-config package to continue)
fi

CXXFLAGS="-O2 -export-dynamic"
OPENLDEV_CFLAGS='-I$(top_srcdir)'
OPENLDEV_LIBS='$(top_builddir)/openldev/libopenldev.la'
PLUGIN_FLAGS='-module -avoid-version'

AC_SUBST(CXXFLAGS)
AC_SUBST(OPENLDEV_CFLAGS)
AC_SUBST(OPENLDEV_LIBS)
AC_SUBST(PLUGIN_FLAGS)

dnl Check for required libraries
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.10.0)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

PKG_CHECK_MODULES(GTKSOURCEVIEW, gtksourceview-1.0 >= 1.4.0)
AC_SUBST(GTKSOURCEVIEW_LIBS)
AC_SUBST(GTKSOURCEVIEW_CFLAGS)

PKG_CHECK_MODULES(GNOMEVFS, gnome-vfs-2.0)
AC_SUBST(GNOMEVFS_LIBS)
AC_SUBST(GNOMEVFS_CFLAGS)

PKG_CHECK_MODULES(VTE, vte)
AC_SUBST(VTE_LIBS)
AC_SUBST(VTE_CFLAGS)

PKG_CHECK_MODULES(LIBGLADE, libglade-2.0)
AC_SUBST(LIBGLADE_LIBS)
AC_SUBST(LIBGLADE_CFLAGS)

PKG_CHECK_MODULES(LIBXML, libxml-2.0)
AC_SUBST(LIBXML_LIBS)
AC_SUBST(LIBXML_CFLAGS)

PKG_CHECK_MODULES(GNOMEPRINT, libgnomeprint-2.2)
AC_SUBST(GNOMEPRINT_LIBS)
AC_SUBST(GNOMEPRINT_CFLAGS)

PKG_CHECK_MODULES(GNOMEPRINTUI, libgnomeprintui-2.2)
AC_SUBST(GNOMEPRINTUI_LIBS)
AC_SUBST(GNOMEPRINTUI_CFLAGS)

PKG_CHECK_MODULES(GCONF, gconf-2.0)
AC_SUBST(GCONF_LIBS)
AC_SUBST(GCONF_CFLAGS)

PKG_CHECK_MODULES(GNOMEUI, libgnomeui-2.0)
AC_SUBST(GNOMEUI_LIBS)
AC_SUBST(GNOMEUI_CFLAGS)

AC_OUTPUT([
Makefile 
glade/Makefile 
menus/Makefile 
openldev/Makefile 
openldev/launcher/Makefile 
openldev/openldev-dialogs/Makefile 
openldev/openldev-settings/Makefile 
openldev/openldev-widgets/Makefile 
pixmaps/Makefile 
plugins/Makefile 
plugins/insert-info/Makefile 
plugins/makefile-editor/Makefile 
templates/Makefile 
templates/console-c/Makefile 
templates/console-cxx/Makefile 
templates/gtk/Makefile
])
