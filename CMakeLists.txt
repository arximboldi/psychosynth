#
#   PSYCHOSYNTH
#   ===========
#
#   Copyright (C) 2007, 2008, 2016 by Juan Pedro Bolivar Puente
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

cmake_policy(SET CMP0048 NEW) # enable project VERSION
cmake_policy(SET CMP0056 NEW) # honor link flags in try_compile()

project(psychosynth VERSION 0.4.0)
cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-local-typedefs")

#  Options
#  =====================================================================

set(WITH_LOCAL_DATA off CACHE bool "use local data")

set(WITH_PSYNTH auto CACHE string "build cli client")
set(WITH_PSYNTH3D auto CACHE string "build 3d gui client")
set(WITH_TESTS auto CACHE string "build unit tests")

set(WITH_OSC auto CACHE string "enable OSC")
set(WITH_XML auto CACHE string "enable XML config")

set(WITH_OGG auto CACHE string "enable ogg")
set(WITH_WAV auto CACHE string "enable other samples")

set(WITH_ALSA auto CACHE string "enable ALSA sound system")
set(WITH_OSS auto CACHE string "enable OSS sound system")
set(WITH_JACK auto CACHE string "enable Jack sound system")

#  Required libraries
#  =====================================================================

find_package(PkgConfig)
include(CheckIncludeFiles)

#  Boost
#  ---------------------------------------------------------------------

set(BOOST_COMPONENTS
  system
  filesystem
  signals)

if(WITH_TESTS)
  if(WITH_TESTS STREQUAL auto)
    find_package(Boost 1.56 REQUIRED COMPONENTS unit_test_framework)
    set(BOOST_TEST_FOUND ${Boost_FOUND})
  else()
    set(BOOST_TEST_FOUND 1)
  endif()
endif()

if(BOOST_TEST_FOUND)
  list(APPEND BOOST_COMPONENTS unit_test_framework)
endif()

find_package(Boost 1.56 REQUIRED COMPONENTS ${BOOST_COMPONENTS})
set(BOOST_TEST_FOUND ${Boost_FOUND})

#  SoundTouch
#  ---------------------------------------------------------------------

pkg_check_modules(SOUNDTOUCH soundtouch REQUIRED)

#  Optional libraries
#  =====================================================================

function(optional_pkg_check_modules cond name expr)
  if (${cond})
    if (NOT (${cond} STREQUAL auto))
      set(is_required REQUIRED)
    endif()
    pkg_check_modules(${name} ${expr} ${is_required})
  endif()
endfunction(optional_pkg_check_modules)

function(optional_check_include_files cond name expr)
  if (${cond})
    check_include_files(${expr} ${name}_FOUND)
    if (NOT (${cond} STREQUAL auto) AND
        NOT ${name}_FOUND)
      message(SEND_ERROR
        "Did not find header file ${expr} "
        "as required by ${cond}" )
    endif()
  endif()
endfunction(optional_check_include_files)

optional_pkg_check_modules(WITH_XML LIBXML libxml-2.0>=2.6)
optional_pkg_check_modules(WITH_OSC LIBLO liblo>=0.23)
optional_pkg_check_modules(WITH_WAV SNDFILE sndfile>=1.0.18)
optional_pkg_check_modules(WITH_OGG VORBIS vorbisfile)

optional_pkg_check_modules(WITH_ALSA ALSA alsa>=1.0.0)
optional_pkg_check_modules(WITH_JACK JACK jack>=0.100)
optional_check_include_files(WITH_OSS OSS sys/soundcard.h)

optional_pkg_check_modules(WITH_PSYNTH3D OGRE OGRE>=1.6)
optional_pkg_check_modules(WITH_PSYNTH3D CEGUI CEGUI-0>=0.7)
optional_pkg_check_modules(WITH_PSYNTH3D CEGUIOGRE CEGUI-0-OGRE>=0.7)
optional_pkg_check_modules(WITH_PSYNTH3D OIS OIS>=1.0)

#  Define targets
#  =====================================================================

function (set_yes_no variable)
  if (${ARGN})
    set(${variable} yes PARENT_SCOPE)
    set(${variable}_P 1 PARENT_SCOPE)
  else()
    set(${variable} no PARENT_SCOPE)
    set(${variable}_P 0 PARENT_SCOPE)
  endif()
endfunction(set_yes_no)

set_yes_no(BUILD_PSYNTH LIBLO_FOUND)
set_yes_no(BUILD_PSYNTH3D
  OGRE_FOUND AND CEGUI_FOUND AND
  CEGUIOGRE_FOUND AND OIS_FOUND)
set_yes_no(BUILD_TESTS BOOST_TEST_FOUND)

set_yes_no(HAVE_OSC LIBLO_FOUND)
set_yes_no(HAVE_XML LIBXML_FOUND)

set_yes_no(HAVE_WAV SNDFILE_FOUND)
set_yes_no(HAVE_OGG VORBIS_FOUND)
set_yes_no(HAVE_SOUNDTOUCH SOUNDTOUCH_FOUND)

set_yes_no(HAVE_OSS OSS_FOUND)
set_yes_no(HAVE_ALSA ALSA_FOUND)
set_yes_no(HAVE_JACK JACK_FOUND)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(data)

#  Summary
#  =====================================================================

message("

  ${PROJECT_NAME} ${PROJECT_VERSION}

    Configuration summmary
")

message("       3d client (psynth3d) ..... ${BUILD_PSYNTH3D}")
if (WITH_PSYNTH3D)
  if (NOT OGRE_FOUND)
    message("           > Ogre >= 1.6 not installed")
  endif()
  if (NOT CEGUI_FOUND)
    message("           > Cegui >= 0.7 not installed")
  endif()
  if (NOT CEGUIOGRE_FOUND)
    message("           > Cegui-Ogre >= 0.7  not installed")
  endif()
  if (NOT OIS_FOUND)
    message("           > OIS >= 1.0 not installed")
  endif()
endif()

message("       CLI client (psynth) ...... ${BUILD_PSYNTH}")
if (WITH_PSYNTH AND NOT LIBLO_FOUND)
  message("           > liblo >= 0.26 not installed")
endif()

message("")

message("       OSC support: ............. ${HAVE_OSC}")
if (WITH_OSC AND NOT LIBLO_FOUND)
  message("           > liblo >= 0.26 not installed")
endif()

message("       XML config support: ...... ${HAVE_XML}")
if (WITH_WAV AND NOT LIBXML_FOUND)
  message("           > libxml >= 2.0 not installed")
endif()

message("       Time-stretch support: .... ${HAVE_SOUNDTOUCH}")
if (NOT SOUNDTOUCH_FOUND)
  message("           > libSoundTouch not installed")
endif()

message("       Sampling support: ........ ${HAVE_WAV}")
if (WITH_WAV AND NOT SNDFILE_FOUND)
  message("           > libsndfile >= 1.0.18 not installed")
endif()

message("       OSS support: ............. ${HAVE_OSS}")
if (WITH_OSS AND NOT OSS_FOUND)
  message("           > no 'sys/soundcard.h' header")
endif()

message("       ALSA support: ............ ${HAVE_ALSA}")
if (WITH_ALSA AND NOT ALSA_FOUND)
  message("           > ALSA >= 1.0 not installed")
endif()

message("       Jack support: ............ ${HAVE_JACK}")
if (WITH_JACK AND NOT JACK_FOUND)
  message("           > Jack >= 0.100 not installed")
endif()

message("")

message("       Unit tests: .............. ${BUILD_TESTS}")
if (WITH_TESTS AND NOT BOOST_TEST_FOUND)
  message("           > ALSA >= 1.0 not installed")
endif()

message("
")
