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
AM_INIT_AUTOMAKE(psychosynth, 0.0.1)

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
        [liblo=$enableval], [liblo=auto])

if test "x${liblo}" != xno ; then
    PKG_CHECK_MODULES(LIBLO, [ liblo >= 0.24 ],
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

#  LibXML support
# ----------------------------------------------------------------------

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

#  Audiofile
# ----------------------------------------------------------------------

AC_ARG_ENABLE([audiofile],
    AC_HELP_STRING([--disable-audiofile], [Disable optional Audiofile support]),
        [audiofile=$enableval], [audiofile=auto])

if test "x${audiofile}" != xno ; then
    PKG_CHECK_MODULES(AUDIOFILE, [ audiofile >= 0.2 ],
        [
            HAVE_AUDIOFILE=yes
            AC_DEFINE([HAVE_AUDIOFILE], 1, [Have Audiofile?])
        ],
        [
            HAVE_AUDIOFILE=no
        ])
else
    HAVE_AUDIOFILE=disabled
fi

#  OSS
# ----------------------------------------------------------------------

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

#  ALSA
# ----------------------------------------------------------------------

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

#  JACK
# ----------------------------------------------------------------------

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


#  psynth3d dependencies
# ======================================================================

AC_ARG_ENABLE([psynth3d],
    AC_HELP_STRING([--disable-psynth3d], [Do not build the 3d client.]),
        [psynth3d=$enableval], [psynth3d=auto])

if test "x${psynth3d}" != xno ; then
   BUILD_PSYNTH3D=yes
   
   # Check OGRE
   PKG_CHECK_MODULES(OGRE, [OGRE >= 1.2], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   
   # Check CEGUI
   if test "x$BUILD_PSYNTH3D" = xyes; then
       PKG_CHECK_MODULES(CEGUI, [CEGUI >= 0.5], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
   
   # Check CEGUI OGRE backend
   if test "x$BUILD_PSYNTH3D" = xyes; then
       PKG_CHECK_MODULES(CEGUIOGRE, [CEGUI-OGRE >= 1.2], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
   
   # Check OIS
   if test "x$BUILD_PSYNTH3D" = xyes; then   
       PKG_CHECK_MODULES(OIS, [OIS >= 1.0], BUILD_PSYNTH3D=yes, BUILD_PSYNTH3D=no)
   fi
else
   BUILD_PSYNTH3D=no
fi

#  psynth dependencies
# ======================================================================

BUILD_PSYNTH=$HAVE_LIBLO

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

#  Conditions
# ======================================================================

if test "x$HAVE_LIBLO" = xyes; then HAVE_LIBLO_P=1
else HAVE_LIBLO_P=0; fi
if test "x$HAVE_LIBXML" = xyes; then HAVE_LIBXML_P=1
else HAVE_LIBXML_P=0; fi
if test "x$HAVE_AUDIOFILE" = xyes; then HAVE_AUDIOFILE_P=1
else HAVE_AUDIOFILE_P=0; fi
if test "x$HAVE_OSS" = xyes; then HAVE_OSS_P=1
else HAVE_OSS_P=0; fi
if test "x$HAVE_ALSA" = xyes; then HAVE_ALSA_P=1
else HAVE_ALSA_P=0; fi
if test "x$HAVE_JACK" = xyes; then HAVE_JACK_P=1
else HAVE_JACK_P=0; fi

AC_SUBST(HAVE_LIBLO_P)
AC_SUBST(HAVE_LIBXML_P)
AC_SUBST(HAVE_AUDIOFILE_P)
AC_SUBST(HAVE_OSS_P)
AC_SUBST(HAVE_ALSA_P)
AC_SUBST(HAVE_JACK_P)

AM_CONDITIONAL([HAVE_LIBLO], [test "x$HAVE_LIBLO" = xyes])
AM_CONDITIONAL([HAVE_LIBXML], [test "x$HAVE_LIBXML" = xyes])
AM_CONDITIONAL([HAVE_AUDIOFILE], [test "x$HAVE_AUDIOFILE" = xyes])
AM_CONDITIONAL([HAVE_OSS], [test "x$HAVE_OSS" = xyes])
AM_CONDITIONAL([HAVE_ALSA], [test "x$HAVE_ALSA" = xyes])
AM_CONDITIONAL([HAVE_JACK], [test "x$HAVE_JACK" = xyes])
AM_CONDITIONAL([BUILD_PSYNTH], [test "x$BUILD_PSYNTH" = xyes])
AM_CONDITIONAL([BUILD_PSYNTH3D], [test "x$BUILD_PSYNTH3D" = xyes])

#  Create files
# ======================================================================

AC_OUTPUT(Makefile 
	  src/Makefile
	  libpsynth/Makefile
	  libpsynth/version.h)


#  Result
# ======================================================================

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

