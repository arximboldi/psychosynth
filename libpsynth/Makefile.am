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

AM_CPPFLAGS =

#  Libraries to build
# ======================================================================

lib_LTLIBRARIES = libpsynth.la

#  libpsynth compilation
# ======================================================================

libpsynth_la_CXXFLAGS =
libpsynth_la_LDFLAGS = 
libpsynth_la_LIBADD =

libpsynth_la_includedir = $(includedir)/libpsynth
nobase_libpsynth_la_include_HEADERS = \
	psynth/Director.h \
	psynth/Defaults.h \
	psynth/DefaultsJack.h \
	psynth/DefaultsOss.h \
	psynth/DefaultsAlsa.h \
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
	common/OptionConf.h \
	common/Timer.h \
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
	output/Output.h\
	version.h

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
	common/Timer.cpp \
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
	object/Oscillator.cpp

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	common/ConfBackendXML.cpp
nobase_libpsynth_la_include_HEADERS += \
	common/ConfBackendXML.h
endif

if HAVE_LIBLO
libpsynth_la_CXXFLAGS += $(LIBLO_CFLAGS)
libpsynth_la_LIBADD += $(LIBLO_LIBS)
nobase_libpsynth_la_include_HEADERS += \
	net/OSCBroadcast.h\
	net/OSCServer.h \
	net/OSCClient.h \
	net/OSCServerLogger.h \
	net/OSCClientLogger.h \
	net/OSCController.h \
	net/OSCMisc.h \
	net/OSCProtocol.h
libpsynth_la_SOURCES += \
	net/OSCBroadcast.cpp \
	net/OSCController.cpp \
	net/OSCServer.cpp \
	net/OSCClient.cpp
endif

if HAVE_ALSA
libpsynth_la_CXXFLAGS += $(ALSA_CFLAGS)
libpsynth_la_LIBADD += $(ALSA_LIBS)
nobase_libpsynth_la_include_HEADERS += \
	psynth/DefaultsAlsa.h \
	psynth/OutputDirectorAlsa.h \
	output/OutputAlsa.h	
libpsynth_la_SOURCES += \
	output/OutputAlsa.cpp
endif

if HAVE_OSS
nobase_libpsynth_la_include_HEADERS += \
	psynth/DefaultsOss.h \
	psynth/OutputDirectorOss.h \
	output/OutputOss.h
libpsynth_la_SOURCES += \
	output/OutputOss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynth_la_include_HEADERS += \
	psynth/DefaultsJack.h \
	psynth/OutputDirectorJack.h \
	output/OutputJack.h
libpsynth_la_SOURCES += \
	output/OutputJack.cpp
endif

if HAVE_AUDIOFILE
libpsynth_la_CXXFLAGS += $(AUDIOFILE_CFLAGS)
libpsynth_la_LIBADD += $(AUDIOFILE_LIBS)
nobase_libpsynth_la_include_HEADERS += \
	output/OutputWave.h
libpsynth_la_SOURCES += \
	output/OutputWave.cpp
endif


libpsynth_la_SOURCES += nobase_libpsynth_la_include_HEADERS
