bin_PROGRAMS = wpage 

AM_CXXFLAGS =  \
	-std=c++11 \
	-rdynamic \
	$(X11_CFLAGS) \
	$(XCB_CFLAGS) \
	$(X11XCB_CFLAGS) \
	$(XCBUTIL_CFLAGS) \
	$(XCBFIXES_CFLAGS) \
	$(XCBDMG_CFLAGS) \
	$(XCBRR_CFLAGS) \
	$(XCBCOMP_CFLAGS) \
	$(CAIRO_CFLAGS) \
	$(PANGO_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(WESTON_CFLAGS) \
	-fno-strict-aliasing

wpage_SOURCES = \
	main.cxx 

wpage_LDADD = \
	$(X11_LIBS) \
	$(XCB_LIBS) \
	$(X11XCB_LIBS) \
	$(XCBUTIL_LIBS) \
	$(XCBFIXES_LIBS) \
	$(XCBDMG_LIBS) \
	$(XCBRR_LIBS) \
	$(XCBCOMP_LIBS) \
	$(CAIRO_LIBS) \
	$(PANGO_LIBS) \
	$(GLIB_LIBS) \
	$(WESTON_LIBS) \
	$(RT_LIBS) 


