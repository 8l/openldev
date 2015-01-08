INCLUDES = \
	$(GTK_CFLAGS)\
	$(GTKSOURCEVIEW_CFLAGS)\
	$(GNOMEVFS_CFLAGS)\
	$(VTE_CFLAGS)\
	$(LIBGLADE_CFLAGS)\
	$(LIBXML_CFLAGS)\
	$(GNOMEPRINT_CFLAGS)\
	$(GNOMEPRINTUI_CFLAGS)\
	$(GCONF_CFLAGS)\
	$(GNOMEUI_CFLAGS)\
	$(OPENLDEV_CFLAGS)

AM_CPPFLAGS = \
	-DINSTALL_PREFIX=\"$(prefix)\"

plugindir = $(libdir)/openldev

plugin_LTLIBRARIES = \
	libinsertinfo.la

libinsertinfo_la_SOURCES = \
	insert-info.cc

libinsertinfo_la_LIBADD = \
	$(GTK_LIBS)\
	$(GTKSOURCEVIEW_LIBS)\
	$(GNOMEVFS_LIBS)\
	$(VTE_LIBS)\
	$(LIBGLADE_LIBS)\
	$(LIBXML_LIBS)\
	$(GNOMEPRINT_LIBS)\
	$(GNOMEPRINTUI_LIBS)\
	$(GCONF_LIBS)\
	$(GNOMEUI_LIBS)\
	$(OPENLDEV_LIBS)

libinsertinfo_la_LDFLAGS = $(PLUGIN_FLAGS)

plugin_DATA = \
	insertinfo.plugin

gladedir = $(prefix)/share/openldev/glade

glade_DATA = \
  inserttimedate.glade

EXTRA_DIST = \
	$(plugin_DATA)\
	$(glade_DATA)
