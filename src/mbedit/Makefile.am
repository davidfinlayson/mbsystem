bin_PROGRAMS = mbedit

AM_CFLAGS = ${libnetcdf_CFLAGS}

AM_CPPFLAGS = -I${top_srcdir}/src/mbio \
	      -I${top_srcdir}/src/mbaux \
	      ${libnetcdf_CPPFLAGS} \
	      ${motif_CPPFLAGS}
if BUILD_GSF
  AM_CPPFLAGS += -I@top_srcdir@/src/gsf
endif

AM_LDFLAGS = 

if BUILD_GSF
  AM_LDFLAGS += -L$(top_builddir)/src/gsf
endif

LIBS = $(top_builddir)/src/mbio/libmbio.la \
       $(top_builddir)/src/mbaux/libmbaux.la \
       $(top_builddir)/src/mbaux/libmbxgr.la \
       ${libgmt_LIBS} ${libnetcdf_LIBS} ${libproj_LIBS} \
       ${motif_LIBS} \
       $(LIBM)

mbedit_SOURCES = mbedit.c mbedit_bxutils.c mbedit_callbacks.c mbedit_creation.c mbedit_prog.c mbedit_creation.h mbedit.h
