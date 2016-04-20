#
#   PSYCHOSYNTH
#   ===========
#
#   Copyright (C) 2009 by Juan Pedro Bolivar Puente
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

install(FILES
  man/psynth-cli.1
  man/psynth3d.1
  DESTINATION ${CMAKE_INSTALL_FULL_MANDIR})

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")

if (HAVE_DOC)
  add_custom_target(doc
    DEPENDS psynth
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
endif()

if (HAVE_MAN)
  add_custom_target(man-psynth3d
    DEPENDS psynth3d
    COMMAND ${HELP2MAN}
    -o "${CMAKE_CURRENT_SOURCE_DIR}/man/psynth3d.1"
    "${CMAKE_BINARY_DIR}/src/psynth3d"
    COMMENT "Updating manpage for psynth3d")

  add_custom_target(man-psynth-cli
    DEPENDS psynth-cli
    COMMAND ${HELP2MAN}
    -o "${CMAKE_CURRENT_SOURCE_DIR}/man/psynth-cli.1"
    "${CMAKE_BINARY_DIR}/src/psynth-cli"
    COMMENT "Updating manpage for psynth-cli"
    VERBATIM)

  add_custom_target(man DEPENDS
    man-psynth3d
    man-psynth-cli)
endif()
