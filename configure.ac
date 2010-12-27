dnl
dnl   PSYCHOSYNTH
dnl   ===========
dnl              
dnl   Copyright (C) 2007, 2008 by Juan Pedro Bolivar Puente
dnl                                                        
dnl   This program is free software: you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation, either version 3 of the License, or
dnl   (at your option) any later version.
dnl                                      
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public License
dnl   along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl

dnl  Global config
dnl ======================================================================

AC_PREREQ(2.61)
AC_INIT([psychosynth], [0.1.7], [raskolnikov@es.gnu.org])
AC_CONFIG_MACRO_DIR([m4])

dnl TODO: Make portable makefiles.
AM_INIT_AUTOMAKE([--warnings=no-portability])
AM_CONFIG_HEADER(config.h)

AC_LANG(C++)
AC_PROG_CXX
AM_PROG_LIBTOOL

psynth_config_ok="yes"

dnl  Options
dnl ======================================================================

dnl CXX=gfilt
CXXFLAGS="${CXXFLAGS} -std=c++0x"

AC_ARG_ENABLE([gprof], 
		[AC_HELP_STRING([--enable-gprof],
	        [enable fprof [default=disabled] ])],
		[CXXFLAGS="${CXXFLAGS} -pg"], [])

AC_ARG_ENABLE([debug], 
		[AC_HELP_STRING([--enable-debug],
	        [enable debug [default=disabled] ])],
		[CXXFLAGS="${CXXFLAGS} -g -O0"], 
		[CXXFLAGS="${CXXFLAGS} -DNDEBUG"])

dnl  Compulsory libraries
dnl ======================================================================

dnl  BOOST
dnl ----------------------------------------------------------------------

AX_BOOST_BASE([1.39])
AX_BOOST_THREAD
AX_BOOST_FILESYSTEM
AX_BOOST_UNIT_TEST_FRAMEWORK

if test "x${ax_cv_boost_thread}" != xyes ; then
AC_MSG_ERROR([
***
*** You need Boost.Thread library to compile the program.
*** Install it and run this script again.
***
])
psynth_config_ok=no
fi

if test "x${ax_cv_boost_filesystem}" != xyes ; then
AC_MSG_ERROR([
***
*** You need Boost.Filsystem library to compile the program.
*** Install it and run this script again.
***
])
psynth_config_ok=no
fi

dnl  Sigc
dnl ----------------------------------------------------------------------
   
PKG_CHECK_MODULES(SIGC, [ sigc++-2.0 ],
[
    HAVE_SIGC=yes
    AC_DEFINE([HAVE_SIGC], 1, [Have Sndfile?])
],
[
AC_MSG_ERROR([
***
*** You need Sigc++ library to compile the program.
*** Install it and run this script again. 
***
])
])


dnl  Gtk 2.0
dnl ----------------------------------------------------------------------

PKG_CHECK_MODULES(GTK, [ gtk+-2.0 ],
[
    HAVE_GTK=yes
    AC_DEFINE([HAVE_GTK], 1, [Have Gtk?])
],
[
AC_MSG_ERROR([
***
*** You need GTK 2.0 development libraries to compile the program.
*** Install it and run this script again.
***
])
])

dnl  Optional libraries
dnl ======================================================================

dnl Unit Testing Framework support
dnl ----------------------------------------------------------------------
HAVE_BOOST_TEST="no"
if test "x${ax_cv_boost_unit_test_framework}" == xyes ; then
   HAVE_BOOST_TEST="yes"
fi

dnl  Liblo support
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([liblo],
    AC_HELP_STRING([--disable-liblo], [Disable optional Liblo support]),
        [liblo=$enableval], [liblo=auto])

if test "x${liblo}" != xno ; then
    PKG_CHECK_MODULES(LIBLO, [ liblo >= 0.23 ],
        [
            HAVE_LIBLO=yes
            AC_DEFINE([HAVE_LIBLO], 1, [Have Liblo?])
        ],
        [
            HAVE_LIBLO=no
        ])
else
    HAVE_LIBLO=disabled
fi

dnl  LibXML support
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([libxml],
    AC_HELP_STRING([--disable-libxml], [Disable optional LibXML support]),
        [libxml=$enableval], [libxml=auto])

if test "x${libxml}" != xno ; then
    PKG_CHECK_MODULES(LIBXML, [ libxml-2.0 >= 2.6 ],
        [
            HAVE_LIBXML=yes
            AC_DEFINE([HAVE_LIBXML], 1, [Have LibXML?])
        ],
        [
            HAVE_LIBXML=no
        ])
else
    HAVE_LIBXML=disabled
fi

dnl  Sndfile
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([sndfile],
    AC_HELP_STRING([--disable-sndfile], [Disable optional Sndfile support]),
        [sndfile=$enableval], [sndfile=auto])

if test "x${sndfile}" != xno ; then
    PKG_CHECK_MODULES(SNDFILE, [ sndfile ],
        [
            HAVE_SNDFILE=yes
            AC_DEFINE([HAVE_SNDFILE], 1, [Have Sndfile?])
        ],
        [
            HAVE_SNDFILE=no
        ])
else
    HAVE_SNDFILE=disabled
fi

dnl  Soundtouch
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([soundtouch],
    AC_HELP_STRING([--disable-soundtouch], [Disable optional Soundtouch support]),
        [soundtouch=$enableval], [soundtouch=auto])

if test "x${soundtouch}" != xno ; then
    PKG_CHECK_MODULES(SOUNDTOUCH, [ soundtouch-1.0 ],
        [
            HAVE_SOUNDTOUCH=yes
            AC_DEFINE([HAVE_SOUNDTOUCH], 1, [Have Soundtouch?])
        ],
        [
            HAVE_SOUNDTOUCH=no
        ])
else
    HAVE_SOUNDTOUCH=disabled
fi

dnl  Samplerate
dnl ----------------------------------------------------------------------

dnl AC_ARG_ENABLE([samplerate],
dnl    AC_HELP_STRING([--disable-samplerate], [Disable optional Samplerate support]),
dnl        [samplerate=$enableval], [samplerate=auto])
dnl
dnl if test "x${samplerate}" != xno ; then
dnl    PKG_CHECK_MODULES(SAMPLERATE, [ samplerate ],
dnl        [
dnl            HAVE_SAMPLERATE=yes
dnl            AC_DEFINE([HAVE_SAMPLERATE], 1, [Have Samplerate?])
dnl        ],
dnl        [
dnl            HAVE_SAMPLERATE=no
dnl        ])
dnl else
dnl    HAVE_SAMPLERATE=disabled
dnl fi

dnl  OGG Vorbis
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([vorbis],
    AC_HELP_STRING([--disable-vorbis], [Disable optional Vorbis support]),
        [vorbis=$enableval], [vorbis=auto])

if test "x${vorbis}" != xno ; then
    PKG_CHECK_MODULES(VORBIS, [ vorbisfile ],
        [
            HAVE_VORBIS=yes
            AC_DEFINE([HAVE_VORBIS], 1, [Have Vorbis?])
        ],
        [
            HAVE_VORBIS=no
        ])
else
    HAVE_VORBIS=disabled
fi

dnl  OSS
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([oss],
    AC_HELP_STRING([--disable-oss], [Disable optional OSS support]),
        [oss=$enableval], [oss=auto])

if test "x${oss}" != xno ; then
    AC_CHECK_HEADERS([sys/soundcard.h],
        [
            HAVE_OSS=yes
            AC_DEFINE([HAVE_OSS], 1, [Have OSS?])
        ],
        [
            HAVE_OSS=no
        ])
else
    HAVE_OSS=disabled
fi

dnl  ALSA
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([alsa],
    AC_HELP_STRING([--disable-alsa], [Disable optional ALSA support]),
        [alsa=$enableval], [alsa=auto])

if test "x${alsa}" != xno ; then
    PKG_CHECK_MODULES(ALSA, [ alsa >= 1.0.0 ],
        [
            HAVE_ALSA=yes
            AC_DEFINE([HAVE_ALSA], 1, [Have ALSA?])
        ],
        [
            HAVE_ALSA=no
        ])
else
    HAVE_ALSA=disabled
fi

dnl  JACK
dnl ----------------------------------------------------------------------

AC_ARG_ENABLE([jack],
    AC_HELP_STRING([--disable-jack], [Disable optional JACK support]),
        [jack=$enableval], [jack=auto])

if test "x${jack}" != xno ; then
    PKG_CHECK_MODULES(JACK, [ jack >= 0.100 ],
        [
            HAVE_JACK=yes
            AC_DEFINE([HAVE_JACK], 1, [Have JACK?])
        ],
        [
            HAVE_JACK=no
        ])
else
    HAVE_JACK=disabled
fi


dnl  psynth3d dependencies
dnl ======================================================================

AC_ARG_ENABLE([psynth3d],
    AC_HELP_STRING([--disable-psynth3d], [Do not build the 3d client.]),
        [psynth3d=$enableval], [psynth3d=auto])

if test "x${psynth3d}" != xno ; then
   BUILD_PSYNTH3D=yes
   
   dnl Check OGRE
   PKG_CHECK_MODULES(OGRE, [OGRE >= 1.2], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   
   dnl Check CEGUI
   if test "x$BUILD_PSYNTH3D" = xyes; then
       PKG_CHECK_MODULES(CEGUI, [CEGUI >= 0.5], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
   
   dnl Check CEGUI OGRE backend
   if test "x$BUILD_PSYNTH3D" = xyes; then
       PKG_CHECK_MODULES(CEGUIOGRE, [CEGUI-OGRE >= 1.2], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
   
   dnl Check OIS
   if test "x$BUILD_PSYNTH3D" = xyes; then   
       PKG_CHECK_MODULES(OIS, [OIS >= 1.0], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
else
   BUILD_PSYNTH3D=no
fi

dnl  psynth dependencies
dnl ======================================================================

BUILD_PSYNTH=$HAVE_LIBLO

dnl  psynth global
dnl ======================================================================

if test "x$HAVE_SOUNDTOUCH" == xno; then
AC_MSG_ERROR([

***
*** You need libSoundTouch to compile the program.
*** Install it and run this script again.
***
])
psynth_config_ok=no
fi


dnl  Directories
dnl ======================================================================

dnl Set PREFIX
dnl if test "x${prefix}" = "xNONE"; then
dnl  prefix=${ac_default_prefix}
dnl  exec_prefix=${ac_default_prefix}
dnl fi

PACKAGE_DATA_DIR="$datarootdir/psychosynth"
PACKAGE_INCLUDE_DIR="$includedir/psynth"
PACKAGE_BIN_DIR=$bindir
PACKAGE_LIB_DIR=$libdir

dnl Uncomment only for development purposes
dnl PACKAGE_DATA_DIR="./data"

AC_SUBST(PACKAGE_DATA_DIR)
AC_SUBST(PACKAGE_BIN_DIR)
AC_SUBST(PACKAGE_INCLUDE_DIR)
AC_SUBST(PACKAGE_LIB_DIR)

dnl  Conditions
dnl ======================================================================

if test "x$HAVE_BOOST_TEST" = xyes; then HAVE_BOOST_TEST_P=1
else HAVE_BOOST_TEST_P=0; fi
if test "x$HAVE_LIBLO" = xyes; then HAVE_LIBLO_P=1
else HAVE_LIBLO_P=0; fi
if test "x$HAVE_LIBXML" = xyes; then HAVE_LIBXML_P=1
else HAVE_LIBXML_P=0; fi
if test "x$HAVE_VORBIS" = xyes; then HAVE_VORBIS_P=1
else HAVE_VORBIS_P=0; fi
if test "x$HAVE_SNDFILE" = xyes; then HAVE_SNDFILE_P=1
else HAVE_SNDFILE_P=0; fi
if test "x$HAVE_SOUNDTOUCH" = xyes; then HAVE_SOUNDTOUCH_P=1
else HAVE_SOUNDTOUCH_P=0; fi
if test "x$HAVE_SAMPLERATE" = xyes; then HAVE_SAMPLERATE_P=1
else HAVE_SAMPLERATE_P=0; fi
if test "x$HAVE_OSS" = xyes; then HAVE_OSS_P=1
else HAVE_OSS_P=0; fi
if test "x$HAVE_ALSA" = xyes; then HAVE_ALSA_P=1
else HAVE_ALSA_P=0; fi
if test "x$HAVE_JACK" = xyes; then HAVE_JACK_P=1
else HAVE_JACK_P=0; fi

AC_SUBST(HAVE_BOOST_TEST_P)
AC_SUBST(HAVE_LIBLO_P)
AC_SUBST(HAVE_LIBXML_P)
AC_SUBST(HAVE_VORBIS_P)
AC_SUBST(HAVE_SNDFILE_P)
AC_SUBST(HAVE_SOUNDTOUCH_P)
AC_SUBST(HAVE_SAMPLERATE_P)
AC_SUBST(HAVE_OSS_P)
AC_SUBST(HAVE_ALSA_P)
AC_SUBST(HAVE_JACK_P)

AM_CONDITIONAL([HAVE_BOOST_TEST], [test "x$HAVE_BOOST_TEST" = xyes])
AM_CONDITIONAL([HAVE_LIBLO], [test "x$HAVE_LIBLO" = xyes])
AM_CONDITIONAL([HAVE_LIBXML], [test "x$HAVE_LIBXML" = xyes])
AM_CONDITIONAL([HAVE_VORBIS], [test "x$HAVE_VORBIS" = xyes])
AM_CONDITIONAL([HAVE_SNDFILE], [test "x$HAVE_SNDFILE" = xyes])
AM_CONDITIONAL([HAVE_SOUNDTOUCH], [test "x$HAVE_SOUNDTOUCH" = xyes])
AM_CONDITIONAL([HAVE_SAMPLERATE], [test "x$HAVE_SAMPLERATE" = xyes])
AM_CONDITIONAL([HAVE_OSS], [test "x$HAVE_OSS" = xyes])
AM_CONDITIONAL([HAVE_ALSA], [test "x$HAVE_ALSA" = xyes])
AM_CONDITIONAL([HAVE_JACK], [test "x$HAVE_JACK" = xyes])
AM_CONDITIONAL([BUILD_PSYNTH], [test "x$BUILD_PSYNTH" = xyes])
AM_CONDITIONAL([BUILD_PSYNTH3D], [test "x$BUILD_PSYNTH3D" = xyes])

dnl  Create files
dnl ======================================================================

AC_OUTPUT(libpsynth.pc
	  Makefile
	  doc/Doxyfile
	  doc/Makefile
	  data/Makefile
	  data/desktop/Makefile
	  data/gui3d/Makefile
	  src/Makefile
	  src/psynth/Makefile
	  src/psynth/version.hpp
	  src/test/Makefile)


dnl  Result
dnl ======================================================================

if test "x${psynth_config_ok}" != xyes; then
AC_MSG_RESULT([
**************************************************************
*
*  Psychosynth $VERSION: Automatic configuration FAILED.
*
*  The file config.log has full details.
*  There are required libraries missing.
*  Install those missing libraries and run this script again.
*
**************************************************************
])
else
AC_MSG_RESULT([
**************************************************************
*
*  Psychosynth $VERSION:  Automatic configuration OK.
*
*
*   Configuration summary:
*
*      3D client (psynth3d) ..... $BUILD_PSYNTH3D
*      CLI client (psynth) ...... $BUILD_PSYNTH
* 
*      OSC support: ............. $HAVE_LIBLO
*      XML config support: ...... $HAVE_LIBXML
*      SoundTouch support: ...... $HAVE_SOUNDTOUCH
*      PCM support: ............. $HAVE_SNDFILE
*      OGG support: ............. $HAVE_VORBIS
*      OSS support: ............. $HAVE_OSS
*      ALSA support: ............ $HAVE_ALSA
*      Jack support: ............ $HAVE_JACK
*      Unit tests support: ...... $HAVE_BOOST_TEST
* 
*   Installation directories:
* 
*      Program: ................. $PACKAGE_BIN_DIR
*      Library: ................. $PACKAGE_LIB_DIR
*      Includes: ................ $PACKAGE_INCLUDE_DIR
*      Data: .................... $PACKAGE_DATA_DIR
*
* 
**
* 
*   Type 'make' to compile.
*   Then type 'make install' to install.
*   
*   Have fun!
* 
*************************************************************
])

fi

