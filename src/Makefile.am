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

AM_CPPFLAGS = $(all_includes)

#  Libraries to build
# ======================================================================

lib_LTLIBRARIES = libpsynth.la

#  Programs to build
# ======================================================================

bin_PROGRAMS =

if BUILD_PSYNTH3D
bin_PROGRAMS += psynth3d
endif

#if BUILD_PSYNTH
#bin_PROGRAMS += psynth
#endif

#  libpsynth compilation
# ======================================================================

libpsynth_la_CXXFLAGS =
libpsynth_la_LDFLAGS = 
libpsynth_la_LIBADD =

libpsynth_la_SOURCES = \
	psynth/Director.cpp \
	psynth/PsychosynthApp.cpp \
	common/ArgParser.cpp \
	common/Error.cpp \
	common/AudioBuffer.cpp \
	common/RingAudioBuffer.cpp \
	common/Mutex.cpp \
	common/Misc.cpp \
	common/Logger.cpp \
	common/Config.cpp \
	table/Table.cpp \
	table/Patcher.cpp \
	table/PatcherDynamic.cpp \
	object/Object.cpp \
	object/ObjectOscillator.cpp \
	object/ObjectMixer.cpp \
	object/ObjectAudioMixer.cpp \
	object/ObjectControlMixer.cpp \
	object/ObjectManager.cpp \
	object/ObjectOutput.cpp \
	object/ObjectLFO.cpp \
	object/ObjectAudioOscillator.cpp \
	object/ObjectFilter.cpp \
	object/Filter.cpp \
	object/WaveTable.cpp \
	object/Oscillator.cpp \
	psynth/Director.h \
	psynth/Defaults.h \
	psynth/OutputDirector.h \
	psynth/PsychosynthApp.h \
	common/ArgParser.h \
	common/Error.h \
	common/Singleton.h \
	common/Thread.h \
	common/Mutex.h \
	common/AudioBuffer.h \
	common/RingAudioBuffer.h \
	common/AudioInfo.h \
        common/MapIterator.h \
	common/Misc.h \
	common/Logger.h \
	common/Config.h \
	table/Table.h \
	table/Patcher.h\
	table/PatcherDynamic.h \
	object/Object.h \
	object/ObjectOscillator.h \
	object/ObjectManager.h \
	object/ObjectOutput.h \
	object/ObjectMixer.h \
	object/ObjectAudioMixer.h \
	object/ObjectControlMixer.h \
	object/ObjectLFO.h \
	object/ObjectAudioOscillator.h \
	object/ObjectFilter.h \
	object/Filter.h \
	object/WaveTable.h \
	object/Oscillator.h \
	object/KnownObjects.h \
	output/Output.h

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	common/ConfBackendXML.cpp \
	common/ConfBackendXML.h
endif

if HAVE_LIBLO
libpsynth_la_CXXFLAGS += $(LIBLO_CFLAGS)
libpsynth_la_LIBADD += $(LIBLO_LIBS)
libpsynth_la_SOURCES += \
	net/OSCBroadcast.cpp \
	net/OSCController.cpp \
	net/OSCServer.cpp \
	net/OSCClient.cpp \
	net/OSCBroadcast.h \
	net/OSCServer.h \
	net/OSCClient.h \
	net/OSCServerLogger.h \
	net/OSCClientLogger.h \
	net/OSCController.h \
	net/OSCMisc.h \
	net/OSCProtocol.h
endif

if HAVE_ALSA
libpsynth_la_CXXFLAGS += $(ALSA_CFLAGS)
libpsynth_la_LIBADD += $(ALSA_LIBS)
libpsynth_la_SOURCES += \
	psynth/DefaultsAlsa.h \
	psynth/OutputDirectorAlsa.h \
	output/OutputAlsa.h \
	output/OutputAlsa.cpp
endif

if HAVE_OSS
libpsynth_la_SOURCES += \
	psynth/DefaultsOss.h \
	psynth/OutputDirectorOss.h \
	output/OutputOss.h \
	output/OutputOss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
libpsynth_la_SOURCES += \
	psynth/DefaultsJack.h \
	psynth/OutputDirectorJack.h \
	output/OutputJack.h \
	output/OutputJack.cpp
endif

if HAVE_AUDIOFILE
libpsynth_la_CXXFLAGS += $(AUDIOFILE_CFLAGS)
libpsynth_la_LIBADD += $(AUDIOFILE_LIBS)
libpsynth_la_SOURCES += \
	output/OutputWave.h \
	output/OutputWave.cpp
endif


#  psynth compilation
# ======================================================================

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
	libpsynth.la

psynth3d_SOURCES = \
	gui3d/CameraControllerRasko.cpp \
	gui3d/QuitWindow.cpp \
	gui3d/InputManager.cpp \
	gui3d/SelectorWindow.cpp \
	gui3d/PsychoSynth3D.cpp \
	gui3d/TaskManager.cpp \
	gui3d/Timer.cpp \
	gui3d/ToggableWindow.cpp \
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
	gui3d/NetworkWindow.cpp \
	gui3d/RecordWindow.cpp \
	gui3d/GuiLogSink.cpp \
	psychosynth.cpp \
	gui3d/CameraController.h \
	gui3d/CameraControllerRasko.h \
	gui3d/CeguiInjecter.h \
	gui3d/QuitWindow.h \
	gui3d/InputManager.h \
	gui3d/PsychoSynth3D.h \
	gui3d/SelectorWindow.h\
	gui3d/TaskManager.h\
	gui3d/Timer.h\
	gui3d/ToggableWindow.h\
	gui3d/Element.h\
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
	gui3d/NetworkWindow.h \
	gui3d/RecordWindow.h \
	gui3d/GuiLogSink.h \
	psychosynth.h

