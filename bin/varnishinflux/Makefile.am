#

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include

bin_PROGRAMS = varnishinflux

varnishinflux_SOURCES = varnishinflux.c \
	varnishinflux_options.h \
	varnishinflux_options.c \
	$(top_srcdir)/lib/libvarnish/vas.c \
	$(top_srcdir)/lib/libvarnish/version.c \
	$(top_srcdir)/lib/libvarnish/vpf.c \
	$(top_srcdir)/lib/libvarnish/vtim.c \
	$(top_srcdir)/lib/libvarnish/flopen.c \
	$(top_srcdir)/lib/libvarnishtools/vut.c

varnishinflux_LDADD = \
	$(top_builddir)/lib/libvarnishcompat/libvarnishcompat.la \
	$(top_builddir)/lib/libvarnishapi/libvarnishapi.la \
	-lm \
	@CURSES_LIB@ ${RT_LIBS} ${PTHREAD_LIBS}

noinst_PROGRAMS = varnishinflux_opt2rst
varnishinflux_opt2rst_SOURCES = \
	varnishinflux_options.h \
	varnishinflux_options.c \
	$(top_srcdir)/lib/libvarnishtools/opt2rst.c
