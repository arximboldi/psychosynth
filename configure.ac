AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(psychosynth, 0.1)

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AM_PROG_LIBTOOL

CPPFLAGS="$CPPFLAGS -Wall"

PKG_CHECK_MODULES(OGRE, [OGRE >= 1.2])
AC_SUBST(OGRE_CFLAGS)
AC_SUBST(OGRE_LIBS)

PKG_CHECK_MODULES(CEGUI, [CEGUI >= 0.5])
AC_SUBST(CEGUI_CFLAGS)
AC_SUBST(CEGUI_LIBS)

PKG_CHECK_MODULES(OIS, [OIS >= 1.0])
AC_SUBST(OIS_CFLAGS)
AC_SUBST(OIS_LIBS)


LIBS="$LIBS -lasound"

dnl TEMPORAL
CPPFLAGS="$CPPFLAGS -I/usr/include/OGRE"

AC_OUTPUT(Makefile src/Makefile)

dnl TEST SDL
dnl ========
dnl SDL_VERSION=1.2.0
dnl AM_PATH_SDL($SDL_VERSION,:,
dnl             AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
dnl CFLAGS="$CFLAGS $SDL_CFLAGS"
dnl LIBS="$LIBS $SDL_LIBS"

dnl AC_CHECK_HEADER([SDL/SDL_image.h], :,
dnl 		[ AC_MSG_ERROR([*** Could not find SDL/SDL_image.h ! Install the SDL_image devel package.]) ])
dnl LIBS="$LIBS -lSDL_image"

dnl TEST OPENGL
dnl ===========
dnl GL_LIBS="-lGL"
dnl GLU_LIBS="-lGLU"
dnl CFLAGS="$CFLAGS $GL_CFLAGS $GLU_CFLAGS"
dnl LIBS="$LIBS $GL_LIBS $GLU_LIBS"

dnl TEST ALSA
dnl =========
dnl AC_CHECK_LIB(asound, snd_pcm_open, have_alsa09=yes, have_alsa09=no)
dnl AC_CHECK_HEADER(alsa/asoundlib.h, , have_alsa09=no)
dnl AM_CONDITIONAL(HAVE_ALSA09,test "x$have_alsa09" = xyes)
dnl if test "x$have_alsa09" = xyes; then
dnl 	ALSA09_LIBS="-lasound"
dnl else
dnl 	ALSA09_LIBS=""
dnl fi


