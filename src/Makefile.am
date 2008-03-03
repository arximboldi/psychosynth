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

AM_CXXFLAGS = -Wall

AM_CPPFLAGS = -I$(src_topdir) $(all_includes)

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

psynth_CXXFLAGS = 

psynth_LDFLAGS = 
psynth_LDADD = \
	../libpsynth/libpsynth.la

psynth_SOURCES = \
	cli/PsychosynthCli.cpp\
	psynth.cpp\
	cli/PsychosynthCli.h

#  psynth3d compilation
# ======================================================================

psynth3d_CXXFLAGS = \
	$(OGRE_CFLAGS) \
	$(CEGUI_CFLAGS) \
	$(OIS_CFLAGS) \
	$(CEGUIOGRE_CFLAGS)

psynth3d_LDFLAGS = 
psynth3d_LDADD = \
	$(OGRE_LIBS) \
	$(CEGUI_LIBS) \
	$(OIS_LIBS) \
	$(CEGUIOGRE_LIBS) \
	../libpsynth/libpsynth.la

psynth3d_SOURCES = \
	gui3d/CameraControllerRasko.cpp \
	gui3d/Psychosynth3D.cpp \
	gui3d/InputManager.cpp \
	gui3d/SelectorWindow.cpp \
	gui3d/QuitWindow.cpp \
	gui3d/ToggableWindow.cpp \
	gui3d/ConfWindow.cpp \
	gui3d/TaskManager.cpp \
	gui3d/Element.cpp \
	gui3d/ElemMainComponent.cpp \
	gui3d/ElemSecondComponent.cpp \
	gui3d/ElementManager.cpp \
	gui3d/ElementProperties.cpp \
	gui3d/TheElements.cpp \
	gui3d/WindowList.cpp \
	gui3d/FlatRing.cpp \
	gui3d/Connection.cpp \
	gui3d/OgreMisc.cpp \
	gui3d/GuiLogSink.cpp \
	psynth3d.cpp \
	gui3d/InputManager.h \
	gui3d/Psychosynth3D.h \
	gui3d/CameraController.h \
	gui3d/CameraControllerRasko.h \
	gui3d/CeguiInjecter.h \
	gui3d/QuitWindow.h \
	gui3d/ConfWindow.h \
	gui3d/SelectorWindow.h\
	gui3d/ToggableWindow.h\
	gui3d/TaskManager.h\
	gui3d/Element.h\
	gui3d/ElementTypes.h\
	gui3d/ElemMainComponent.h \
	gui3d/ElemSecondComponent.h \
	gui3d/ElementManager.h\
	gui3d/ElementProperties.h \
	gui3d/TheElements.h \
	gui3d/QueryFlags.h\
	gui3d/WindowList.h\
	gui3d/FlatRing.h\
	gui3d/Connection.h\
	gui3d/OgreMisc.h \
	gui3d/GuiLogSink.h 

if HAVE_LIBXML
psynth3d_CXXFLAGS += $(LIBXML_CFLAGS)
psynth3d_LDADD += $(LIBXML_LIBS)
endif

if HAVE_LIBLO
psynth3d_CXXFLAGS += $(LIBLO_CFLAGS)
psynth3d_LDADD += $(LIBLO_LIBS)
psynth3d_SOURCES += \
	gui3d/NetworkWindow.cpp \
	gui3d/NetworkWindow.h
endif

if HAVE_AUDIOFILE
psynth3d_CXXFLAGS += $(AUDIOFILE_CFLAGS)
psynth3d_LDADD += $(AUDIOFILE_LIBS)
psynth3d_SOURCES += \
	gui3d/RecordWindow.cpp \
	gui3d/RecordWindow.h
endif
