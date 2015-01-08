SUBDIRS = \
	openldev-settings\
	openldev-dialogs\
	openldev-widgets\
	launcher

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
	-DINSTALL_PREFIX=\"$(prefix)\"\
	-DLOCALE_DIR=\""$(datadir)/locale"\"

lib_LTLIBRARIES = \
	libopenldev.la

libopenldev_la_SOURCES = \
	openldev.cc\
	openldev-auto-complete.cc\
	openldev-build.cc\
	openldev-edit.cc\
	openldev-file.cc\
	openldev-gconf.cc\
	openldev-help.cc\
	openldev-plugin.cc\
	openldev-plugin-engine.cc\
	openldev-printing.cc\
	openldev-project.cc\
	openldev-tools.cc\
	openldev-utils.cc\
	openldev-view.cc

libopenldev_la_LIBADD = \
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
	openldev-settings/libsettings.la\
	openldev-dialogs/libdialogs.la\
	openldev-widgets/libwidgets.la

bin_PROGRAMS = \
	openldev

openldev_SOURCES = \
	main.cc

openldev_LDADD = \
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
	libopenldev.la
