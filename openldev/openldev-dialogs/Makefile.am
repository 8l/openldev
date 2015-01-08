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
	libdialogs.la

libdialogs_la_SOURCES = \
	openldev-about.cc\
	openldev-environment.cc\
	openldev-import-project.cc\
	openldev-input.cc\
	openldev-new-file.cc\
	openldev-new-project.cc\
	openldev-project-info.cc\
	openldev-project-settings.cc

libdialogs_la_LIBADD = \
	$(GTK_LIBS)\
	$(GTKSOURCEVIEW_LIBS)\
	$(GNOMEVFS_LIBS)\
	$(VTE_LIBS)\
	$(LIBGLADE_LIBS)\
	$(LIBXML_LIBS)\
	$(GNOMEPRINT_LIBS)\
	$(GNOMEPRINTUI_LIBS)\
	$(GCONF_LIBS)
