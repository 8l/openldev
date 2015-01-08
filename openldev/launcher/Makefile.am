INCLUDES = \
	$(GTK_CFLAGS)

AM_CPPFLAGS = \
	-DINSTALL_PREFIX=\"$(prefix)\"

bin_PROGRAMS = \
	openldev_launcher

openldev_launcher_SOURCES = \
	main.cc

openldev_launcher_LDADD = \
	$(GTK_LIBS)
	
LIBTOOL = \
	$(SHELL)\
	$(top_builddir)/libtool
