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

add_subdirectory(psynth)
add_subdirectory(test)

if (BUILD_PSYNTH)
  add_executable(psynth-cli
    cli/psychosynth_cli.cpp
    cli/psychosynth_cli.hpp
    psynth.cpp)
  target_link_libraries(psynth-cli PUBLIC psynth)
endif()

if (BUILD_PSYNTH3D)
  set(psynth3d_sources
    gui3d/camera_controller.hpp
    gui3d/camera_controller_default.cpp
    gui3d/camera_controller_default.hpp
    gui3d/cegui_injecter.hpp
    gui3d/conf_window.cpp
    gui3d/conf_window.hpp
    gui3d/connection.cpp
    gui3d/connection.hpp
    gui3d/default_selector_populator.cpp
    gui3d/default_selector_populator.hpp
    gui3d/elem_main_component.cpp
    gui3d/elem_main_component.hpp
    gui3d/elem_second_component.cpp
    gui3d/elem_second_component.hpp
    gui3d/elem_toggler_component.cpp
    gui3d/elem_toggler_component.hpp
    gui3d/element.cpp
    gui3d/element.hpp
    gui3d/element_manager.cpp
    gui3d/element_manager.hpp
    gui3d/element_properties.cpp
    gui3d/element_properties.hpp
    gui3d/element_types.hpp
    gui3d/flat_ring.cpp
    gui3d/flat_ring.hpp
    gui3d/gui_log_sink.cpp
    gui3d/gui_log_sink.hpp
    gui3d/info_window.cpp
    gui3d/info_window.hpp
    gui3d/input_manager.cpp
    gui3d/input_manager.hpp
    gui3d/ogre_misc.cpp
    gui3d/ogre_misc.hpp
    gui3d/psychosynth_3d.cpp
    gui3d/psychosynth_3d.hpp
    gui3d/query_flags.hpp
    gui3d/quit_window.cpp
    gui3d/quit_window.hpp
    gui3d/selector_window.cpp
    gui3d/selector_window.hpp
    gui3d/task_manager.cpp
    gui3d/task_manager.hpp
    gui3d/the_elements.cpp
    gui3d/the_elements.hpp
    gui3d/toggable_window.cpp
    gui3d/toggable_window.hpp
    gui3d/window_list.cpp
    gui3d/window_list.hpp
    psynth3d.cpp)

  if (HAVE_OSC)
    list(APPEND psynth3d_sources
      gui3d/network_window.cpp
      gui3d/network_window.hpp)
  endif()

  if (HAVE_WAV)
    list(APPEND psynth3d_sources
      gui3d/record_window.cpp
      gui3d/record_window.hpp)
  endif()

  add_executable(psynth3d ${psynth3d_sources})
  target_compile_options(psynth3d PUBLIC
    ${OGRE_CFLAGS}
    ${OIS_CFLAGS}
    ${CEGUI_CFLAGS}
    ${CEGUIOGRE_CFLAGS})
  target_link_libraries(psynth3d PUBLIC
    psynth
    ${OGRE_LDFLAGS}
    ${OIS_LDFLAGS}
    ${CEGUI_LDFLAGS}
    ${CEGUIOGRE_LDFLAGS})
endif()
