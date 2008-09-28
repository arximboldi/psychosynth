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

#  Common
# ======================================================================

AM_CPPFLAGS = -Wall $(BOOST_CPPFLAGS) $(all_includes)
AM_LDFLAGS  = $(BOOST_LDFLAGS) $(SIGC_LDFLAGS)

#  Programs to build
# ======================================================================

bin_PROGRAMS =

if BUILD_PSYNTH3D
bin_PROGRAMS += psynth3d
endif

if BUILD_PSYNTH
bin_PROGRAMS += psynth
endif

#  psynth compilation
# ======================================================================

psynth_CXXFLAGS = $(SIGC_CFLAGS)
psynth_LDFLAGS = 
psynth_LDADD = $(SIGC_LIBS) \
	../libpsynth/libpsynth.la

psynth_SOURCES = \
	cli/psychosynth_cli.cpp\
	psynth.cpp\
	cli/psychosynth_cli.h

#  psynth3d compilation
# ======================================================================

psynth3d_CXXFLAGS = \
	$(SIGC_CFLAGS)\
	$(OGRE_CFLAGS) \
	$(CEGUI_CFLAGS) \
	$(OIS_CFLAGS) \
	$(CEGUIOGRE_CFLAGS)

psynth3d_LDFLAGS = 
psynth3d_LDADD = \
	$(BOOST_SIGNALS_LIB) \
	$(SIGC_LIBS) \
	$(OGRE_LIBS) \
	$(CEGUI_LIBS) \
	$(OIS_LIBS) \
	$(CEGUIOGRE_LIBS) \
	../libpsynth/libpsynth.la

psynth3d_SOURCES = \
	gui3d/camera_controller_default.cpp \
	gui3d/psychosynth_3d.cpp \
	gui3d/input_manager.cpp \
	gui3d/selector_window.cpp \
	gui3d/default_selector_populator.cpp\
	gui3d/quit_window.cpp \
	gui3d/info_window.cpp \
	gui3d/toggable_window.cpp \
	gui3d/conf_window.cpp \
	gui3d/task_manager.cpp \
	gui3d/element.cpp \
	gui3d/elem_main_component.cpp \
	gui3d/elem_second_component.cpp \
	gui3d/elem_toggler_component.cpp \
	gui3d/element_manager.cpp \
	gui3d/element_properties.cpp \
	gui3d/the_elements.cpp \
	gui3d/window_list.cpp \
	gui3d/flat_ring.cpp \
	gui3d/connection.cpp \
	gui3d/ogre_misc.cpp \
	gui3d/gui_log_sink.cpp \
	psynth3d.cpp \
	gui3d/input_manager.h \
	gui3d/psychosynth_3d.h \
	gui3d/camera_controller.h \
	gui3d/camera_controller_default.h \
	gui3d/cegui_injecter.h \
	gui3d/quit_window.h \
	gui3d/conf_window.h \
	gui3d/selector_window.h\
	gui3d/info_window.h\
	gui3d/default_selector_populator.h\
	gui3d/toggable_window.h\
	gui3d/task_manager.h\
	gui3d/element.h\
	gui3d/element_types.h\
	gui3d/elem_main_component.h \
	gui3d/elem_second_component.h \
	gui3d/elem_toggler_component.h \
	gui3d/element_manager.h\
	gui3d/element_properties.h \
	gui3d/the_elements.h \
	gui3d/query_flags.h\
	gui3d/window_list.h\
	gui3d/flat_ring.h\
	gui3d/connection.h\
	gui3d/ogre_misc.h \
	gui3d/gui_log_sink.h 

if HAVE_LIBXML
psynth3d_CXXFLAGS += $(LIBXML_CFLAGS)
psynth3d_LDADD += $(LIBXML_LIBS)
endif

if HAVE_LIBLO
psynth3d_CXXFLAGS += $(LIBLO_CFLAGS)
psynth3d_LDADD += $(LIBLO_LIBS)
psynth3d_SOURCES += \
	gui3d/network_window.cpp \
	gui3d/network_window.h
endif

if HAVE_SNDFILE
psynth3d_CXXFLAGS += $(SNDFILE_CFLAGS)
psynth3d_LDADD += $(SNDFILE_LIBS)
psynth3d_SOURCES += \
	gui3d/record_window.cpp \
	gui3d/record_window.h
endif
