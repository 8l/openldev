INCLUDES = \
	$(GTK_CFLAGS)\
	$(GTKSOURCEVIEW_CFLAGS)\
	$(GNOMEVFS_CFLAGS)\
	$(VTE_CFLAGS)\
	$(LIBGLADE_CFLAGS)\
	$(LIBXML_CFLAGS)\
	$(GNOMEPRINT_CFLAGS)\
	$(GNOMEPRINTUI_CFLAGS)\
	$(GCONF_CFLAGS)

AM_CPPFLAGS = \
	-DINSTALL_PREFIX=\"$(prefix)\"

noinst_LTLIBRARIES = \
	libsettings.la

libsettings_la_SOURCES = \
	openldev-editor.cc\
	openldev-settings.cc\
	openldev-makefile-parser.cc\
	openldev-project.cc\
	openldev-xml-parser.cc

libsettings_la_LIBADD = \
	$(GTK_LIBS)\
	$(GTKSOURCEVIEW_LIBS)\
	$(GNOMEVFS_LIBS)\
	$(VTE_LIBS)\
	$(LIBGLADE_LIBS)\
	$(LIBXML_LIBS)\
	$(GNOMEPRINT_LIBS)\
	$(GNOMEPRINTUI_LIBS)\
	$(GCONF_LIBS)
