#
#   PSYCHOSYNTH
#   ===========
#              
#   Copyright (C) 2007, 2008 by Juan Pedro Bolivar Puente
#                                                        
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#                                      
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#  Global config
# ======================================================================

AC_INIT(configure.ac)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(psychosynth, 0.1)

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AM_PROG_LIBTOOL

psynth_config_ok="yes"

#  Optional libraries
# ======================================================================

#  Liblo support
# ----------------------------------------------------------------------

AC_ARG_ENABLE([liblo],
    AC_HELP_STRING([--disable-liblo], [Disable optional Liblo support]),
        [
            case "${enableval}" in
                yes) liblo=yes ;;
                no) liblo=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-liblo) ;;
            esac
        ],
        [liblo=auto])

if test "x${liblo}" != xno ; then
    PKG_CHECK_MODULES(LIBLO, [ liblo >= 0.24 ],
        [
            HAVE_LIBLO=1
            AC_DEFINE([HAVE_LIBLO], 1, [Have Liblo?])
        ],
        [
            HAVE_LIBLO=0
            if test "x$liblo" = xyes ; then
                AC_MSG_ERROR([*** Liblo support not found])
		psynth_config_ok = no
            fi
        ])
else
    HAVE_LIBLO=0
fi

AC_SUBST(LIBLO_CFLAGS)
AC_SUBST(LIBLO_LIBS)
AC_SUBST(HAVE_LIBLO)
AM_CONDITIONAL([HAVE_LIBLO], [test "x$HAVE_LIBLO" = x1])


#  LibXML support
# ----------------------------------------------------------------------

AC_ARG_ENABLE([libxml],
    AC_HELP_STRING([--disable-libxml], [Disable optional LibXML support]),
        [
            case "${enableval}" in
                yes) libxml=yes ;;
                no) libxml=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-libxml) ;;
            esac
        ],
        [libxml=auto])

if test "x${libxml}" != xno ; then
    PKG_CHECK_MODULES(LIBXML, [ libxml-2.0 >= 2.6 ],
        [
            HAVE_LIBXML=1
            AC_DEFINE([HAVE_LIBXML], 1, [Have LibXML?])
        ],
        [
            HAVE_LIBXML=0
            if test "x$libxml" = xyes ; then
                AC_MSG_ERROR([*** LibXML support not found])
            fi
        ])
else
    HAVE_LIBXML=0
fi

AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)
AC_SUBST(HAVE_LIBXML)
AM_CONDITIONAL([HAVE_LIBXML], [test "x$HAVE_LIBXML" = x1])


#  Audiofile
# ----------------------------------------------------------------------

AC_ARG_ENABLE([audiofile],
    AC_HELP_STRING([--disable-audiofile], [Disable optional Audiofile support]),
        [
            case "${enableval}" in
                yes) audiofile=yes ;;
                no) audiofile=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-oss) ;;
            esac
        ],
        [audiofile=auto])

if test "x${audiofile}" != xno ; then
    PKG_CHECK_MODULES(AUDIOFILE, [ audiofile >= 0.2 ],
        [
            HAVE_AUDIOFILE=1
            AC_DEFINE([HAVE_AUDIOFILE], 1, [Have Audiofile?])
        ],
        [
            HAVE_AUDIOFILE=0
            if test "x$audiofile" = xyes ; then
                AC_MSG_ERROR([*** Audiofile support not found])
            fi
        ])
else
    HAVE_AUDIOFILE=0
fi

AC_SUBST(AUDIOFILE_CFLAGS)
AC_SUBST(AUDIOFILE_LIBS)
AC_SUBST(HAVE_AUDIOFILE)
AM_CONDITIONAL([HAVE_AUDIOFILE], [test "x$HAVE_AUDIOFILE" = x1])


#  OSS
# ----------------------------------------------------------------------

AC_ARG_ENABLE([oss],
    AC_HELP_STRING([--disable-oss], [Disable optional OSS support]),
        [
            case "${enableval}" in
                yes) oss=yes ;;
                no) oss=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-oss) ;;
            esac
        ],
        [oss=auto])

if test "x${oss}" != xno ; then
    AC_CHECK_HEADERS([sys/soundcard.h],
        [
            HAVE_OSS=1
            AC_DEFINE([HAVE_OSS], 1, [Have OSS?])
        ],
        [
            HAVE_OSS=0
            if test "x$oss" = xyes ; then
                AC_MSG_ERROR([*** OSS support not found])
            fi
        ])
else
    HAVE_OSS=0
fi

AC_SUBST(HAVE_OSS)
AM_CONDITIONAL([HAVE_OSS], [test "x$HAVE_OSS" = x1])


#  ALSA
# ----------------------------------------------------------------------

AC_ARG_ENABLE([alsa],
    AC_HELP_STRING([--disable-alsa], [Disable optional ALSA support]),
        [
            case "${enableval}" in
                yes) alsa=yes ;;
                no) alsa=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-alsa) ;;
            esac
        ],
        [alsa=auto])

if test "x${alsa}" != xno ; then
    PKG_CHECK_MODULES(ALSA, [ alsa >= 1.0.0 ],
        [
            HAVE_ALSA=1
            AC_DEFINE([HAVE_ALSA], 1, [Have ALSA?])
        ],
        [
            HAVE_ALSA=0
            if test "x$alsa" = xyes ; then
                AC_MSG_ERROR([*** ALSA support not found])
            fi
        ])
else
    HAVE_ALSA=0
fi

AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)
AC_SUBST(HAVE_ALSA)
AM_CONDITIONAL([HAVE_ALSA], [test "x$HAVE_ALSA" = x1])


#  JACK
# ----------------------------------------------------------------------

AC_ARG_ENABLE([jack],
    AC_HELP_STRING([--disable-jack], [Disable optional JACK support]),
        [
            case "${enableval}" in
                yes) jack=yes ;;
                no) jack=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-jack) ;;
            esac
        ],
        [jack=auto])

if test "x${jack}" != xno ; then
    PKG_CHECK_MODULES(JACK, [ jack >= 0.100 ],
        HAVE_JACK=1,
        [
            HAVE_JACK=0
            if test "x$jack" = xyes ; then
                AC_MSG_ERROR([*** JACK support not found])
            fi
        ])
else
    HAVE_JACK=0
fi

AC_SUBST(JACK_CFLAGS)
AC_SUBST(JACK_LIBS)
AC_SUBST(HAVE_JACK)
AM_CONDITIONAL([HAVE_JACK], [test "x$HAVE_JACK" = x1])

#  psynth3d dependencies
# ======================================================================

AC_ARG_ENABLE([psynth3d],
    AC_HELP_STRING([--disable-psynth3d], [Do not build the 3d client.]),
        [
            case "${enableval}" in
                yes) psynth3d=yes ;;
                no) psynth3d=no ;;
                *) AC_MSG_ERROR(bad value ${enableval} for --disable-psynth3d) ;;
            esac
        ],
        [liblo=auto])

if test "x${psynth3d}" != xno ; then
   BUILD_PSYNTH3D=1
   
   # Check OGRE
   PKG_CHECK_MODULES(OGRE, [OGRE >= 1.2], BUILD_PSYNTH3D=1, BUILD_PSYNTH3D=0)
   
   # Check CEGUI
   if test "x$BUILD_PSYNTH3D" = x1; then
       PKG_CHECK_MODULES(CEGUI, [CEGUI >= 0.5], BUILD_PSYNTH3D=1, BUILD_PSYNTH3D=0)
   fi
   
   # Check CEGUI OGRE backend
   if test "x$BUILD_PSYNTH3D" = x1; then
       PKG_CHECK_MODULES(CEGUIOGRE, [CEGUI-OGRE >= 1.2], BUILD_PSYNTH3D=1, BUILD_PSYNTH3D=0)
   fi
   
   # Check OIS
   if test "x$BUILD_PSYNTH3D" = x1; then   
       PKG_CHECK_MODULES(OIS, [OIS >= 1.0], BUILD_PSYNTH3D=1, BUILD_PSYNTH3D=0)
   fi
else
   BUILD_PSYNTH3D=0
fi

AM_CONDITIONAL([BUILD_PSYNTH3D], [test "x$BUILD_PSYNTH3D" = x1])

#  psynth dependencies
# ======================================================================

BUILD_PSYNTH=$HAVE_LIBLO
AM_CONDITIONAL([BUILD_PSYNTH], [test "x$BUILD_PSYNTH" = x1])

#  Directories
# ======================================================================

# Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi


PACKAGE_DATA_DIR="${datarootdir}/${PACKAGE}"
AC_SUBST(PACKAGE_DATA_DIR)


#  Create files
# ======================================================================

AC_OUTPUT(Makefile 
	  src/Makefile
	  libpsynth/Makefile)


#  Result
# ======================================================================

if test "x${BUILD_PSYNTH3D}" = x1; then BUILD_PSYNTH3D=yes;
else BUILD_PSYNTH3D=no; fi

if test "x${BUILD_PSYNTH}" = x1; then BUILD_PSYNTH=yes;
else BUILD_PSYNTH=no; fi 

if test "x${HAVE_LIBLO}" = x1; then HAVE_LIBLO=yes;
else HAVE_LIBLO=no; fi

if test "x${HAVE_LIBXML}" = x1; then HAVE_LIBXML=yes;
else HAVE_LIBLO=no; fi

if test "x${HAVE_OSS}" = x1; then HAVE_OSS=yes;
else HAVE_OSS=no; fi

if test "x${HAVE_ALSA}" = x1; then HAVE_ALSA=yes;
else HAVE_ALSA=no; fi

if test "x${HAVE_JACK}" = x1; then HAVE_JACK=yes;
else HAVE_JACK=no; fi

if test "x${HAVE_AUDIOFILE}" = x1; then HAVE_AUDIOFILE=yes;
else HAVE_AUDIOFILE=no; fi


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
*      Audio recording: ......... $HAVE_AUDIOFILE
*      OSS support: ............. $HAVE_OSS
*      ALSA support: ............ $HAVE_ALSA
*      Jack support: ............ $HAVE_JACK
*      Translations: ............ Not suported.
* 
*   Installation directories:
* 
*      Program: ................. ${packageprefix}/bin
*      Data: .................... ${packageprefix}/share
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

