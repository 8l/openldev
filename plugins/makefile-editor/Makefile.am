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

imagedir = $(prefix)/share/pixmaps/openldev

image_DATA = \
  mfeditor-directory.png\
  mfeditor-prefix.png\
  mfeditor-target.png\
  mfeditor-script.png\
  mfeditor-variable.png

gladedir = $(prefix)/share/openldev/glade

glade_DATA = \
  mfeditor-variable.glade\
  mfeditor-target.glade\
  mfeditor-subproject.glade\
  mfeditor-script.glade\
  mfeditor-add.glade\
  mfeditor-prefix.glade

plugindir = $(libdir)/openldev

plugin_LTLIBRARIES = \
	libmakefileeditor.la

libmakefileeditor_la_SOURCES = \
	makefile-editor.cc\
	makefile-editor-edit.cc\
	makefile-editor-add.cc

libmakefileeditor_la_LIBADD = \
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

libmakefileeditor_la_LDFLAGS = $(PLUGIN_FLAGS)

plugin_DATA = \
	makefileeditor.plugin

EXTRA_DIST = \
	$(plugin_DATA)\
	$(image_DATA)\
	$(glade_DATA)
