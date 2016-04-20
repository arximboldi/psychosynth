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

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/gui3d/plugins.cfg.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/gui3d/plugins.cfg")

install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/desktop/psynth3d.desktop"
  DESTINATION "${PSYNTH_DATAROOTDIR}/applications")

install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/desktop/psychosynth.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/desktop/psychosynth.xpm"
  DESTINATION "${PSYNTH_DATAROOTDIR}/pixmaps")

install(DIRECTORY gui3d DESTINATION "${PSYNTH_DATADIR}"
  FILES_MATCHING
  PATTERN "*.cfg"
  PATTERN "*.cg"
  PATTERN "*.font"
  PATTERN "*.imageset"
  PATTERN "*.jpg"
  PATTERN "*.layout"
  PATTERN "*.looknfeel"
  PATTERN "*.material"
  PATTERN "*.mesh"
  PATTERN "*.png"
  PATTERN "*.scheme"
  PATTERN "*.tga"
  PATTERN "*.ttf"
  PATTERN "*.xsd")

install(DIRECTORY samples DESTINATION "${PSYNTH_DATADIR}"
  FILES_MATCHING
  PATTERN "*.wav"
  PATTERN "*.ogg")
