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
	$(GNOMEUI_CFLAGS)

AM_CPPFLAGS = \
	-DINSTALL_PREFIX=\"$(prefix)\"

noinst_LTLIBRARIES = \
	libwidgets.la

libwidgets_la_SOURCES = \
  openldev-file-browser.cc\
	openldev-file-manager.cc\
	openldev-file-tab.cc\
	openldev-find-toolbar.cc\
	openldev-goto-line-toolbar.cc\
	openldev-menu.cc\
	openldev-output-build.cc\
	openldev-output-search.cc\
	openldev-output-terminal.cc\
	openldev-output-manager.cc\
	openldev-sidebar.cc\
	openldev-status-bar.cc

libwidgets_la_LIBADD = \
	$(GTK_LIBS)\
	$(GTKSOURCEVIEW_LIBS)\
	$(GNOMEVFS_LIBS)\
	$(VTE_LIBS)\
	$(LIBGLADE_LIBS)\
	$(LIBXML_LIBS)\
	$(GNOMEPRINT_LIBS)\
	$(GNOMEPRINTUI_LIBS)\
	$(GCONF_LIBS)\
	$(GNOMEUI_LIBS)
