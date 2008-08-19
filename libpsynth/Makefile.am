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

libpsynthincdir = @PACKAGE_INCLUDE_DIR@

nobase_libpsynthinc_HEADERS = \
	psynth/Director.h \
	psynth/Defaults.h \
	psynth/DefaultsJack.h \
	psynth/DefaultsOss.h \
	psynth/DefaultsAlsa.h \
	psynth/OutputDirector.h \
	psynth/FileManagerDirector.h \
	psynth/PsychosynthApp.h \
	common/arg_parser.h \
	common/deleter.h \
	common/error.h \
	common/singleton.h \
	common/thread.h \
	common/mutex.h \
	common/audio_buffer.h \
	common/sample_buffer.h\
	common/FastDelegate.h\
	common/FastDelegateBind.h\
	common/ring_buffer.h\
	common/ring_audio_buffer.h \
	common/ring_sample_buffer.h \
	common/audio_info.h \
        common/map_iterator.h \
	common/misc.h \
	common/logger.h \
	common/config.h \
	common/option_conf.h \
	common/timer.h \
	common/vector_2d.h\
	common/scaler.h\
	common/file_finder.h\
	common/file_manager.h\
	common/tree.h\
	table/Table.h \
	table/Patcher.h\
	table/PatcherDynamic.h \
	table/TableObjectCreator.h\
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
	object/ObjectSampler.h \
	object/ObjectStepSeq.h \
	object/ObjectNoise.h \
	object/ObjectAudioNoise.h \
	object/ObjectControlNoise.h \
	object/ObjectEcho.h \
	object/ObjectDelay.h \
	object/Filter.h \
	object/WaveTable.h \
	object/Oscillator.h \
	object/KnownObjects.h \
	object/ObjParam.h\
	object/EnvelopeSimple.h\
	object/EnvelopeMulti.h\
	object/Envelope.h\
	object/Watch.h\
	object/WatchViewer.h\
	object/ObjectFactory.h\
	object/ObjectFactoryManager.h\
	output/Output.h\
	input/FileReader.h\
	input/FileReaderAny.h\
	input/FileReaderFetcher.h\
	version.h

libpsynth_la_SOURCES = \
	psynth/Director.cpp \
	psynth/PsychosynthApp.cpp \
	psynth/FileManagerDirector.cpp \
	common/arg_parser.cpp \
	common/audio_buffer.cpp \
	common/sample_buffer.cpp \
	common/ring_buffer.cpp \
	common/ring_audio_buffer.cpp \
	common/ring_sample_buffer.cpp \
	common/mutex.cpp \
	common/misc.cpp \
	common/logger.cpp \
	common/config.cpp \
	common/timer.cpp \
	common/file_finder.cpp\
	common/file_manager.cpp\
	table/Table.cpp \
	table/Patcher.cpp \
	table/PatcherDynamic.cpp \
	table/TableObjectCreator.cpp\
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
	object/ObjectSampler.cpp \
	object/ObjectStepSeq.cpp \
	object/ObjectNoise.cpp \
	object/ObjectAudioNoise.cpp \
	object/ObjectControlNoise.cpp \
	object/ObjectEcho.cpp \
	object/ObjectDelay.cpp \
	object/Filter.cpp \
	object/WaveTable.cpp \
	object/Oscillator.cpp\
	object/ObjParam.cpp\
	object/ObjectFactoryManager.cpp\
	object/EnvelopeMulti.cpp\
	input/FileReaderAny.cpp\
	input/FileReaderFetcher.cpp

if HAVE_SAMPLERATE
libpsynth_la_CXXFLAGS += $(SAMPLERATE_CFLAGS)
libpsynth_la_LIBADD += $(SAMPLERATE_LIBS)
libpsynth_la_SOURCES += \
	common/scaler_src.cpp
nobase_libpsynthinc_HEADERS += \
	common/scaler_src.h
endif

if HAVE_SOUNDTOUCH
libpsynth_la_CXXFLAGS += $(SOUNDTOUCH_CFLAGS)
libpsynth_la_LIBADD += $(SOUNDTOUCH_LIBS)
libpsynth_la_SOURCES += \
	common/scaler_st.cpp
nobase_libpsynthinc_HEADERS += \
	common/scaler_st.h
endif

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	common/conf_backend_xml.cpp
nobase_libpsynthinc_HEADERS += \
	common/conf_backend_xml.h
endif

if HAVE_LIBLO
libpsynth_la_CXXFLAGS += $(LIBLO_CFLAGS)
libpsynth_la_LIBADD += $(LIBLO_LIBS)
nobase_libpsynthinc_HEADERS += \
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
nobase_libpsynthinc_HEADERS += \
	psynth/DefaultsAlsa.h \
	psynth/OutputDirectorAlsa.h \
	output/OutputAlsa.h	
libpsynth_la_SOURCES += \
	output/OutputAlsa.cpp
endif

if HAVE_OSS
nobase_libpsynthinc_HEADERS += \
	psynth/DefaultsOss.h \
	psynth/OutputDirectorOss.h \
	output/OutputOss.h
libpsynth_la_SOURCES += \
	output/OutputOss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynthinc_HEADERS += \
	psynth/DefaultsJack.h \
	psynth/OutputDirectorJack.h \
	output/OutputJack.h
libpsynth_la_SOURCES += \
	output/OutputJack.cpp
endif

if HAVE_SNDFILE
libpsynth_la_CXXFLAGS += $(SNDFILE_CFLAGS)
libpsynth_la_LIBADD += $(SNDFILE_LIBS)
nobase_libpsynthinc_HEADERS += \
	output/OutputWave.h\
	input/FileReaderWave.h
libpsynth_la_SOURCES += \
	output/OutputWave.cpp\
	input/FileReaderWave.cpp
endif

if HAVE_VORBIS
libpsynth_la_CXXFLAGS += $(VORBIS_CFLAGS)
libpsynth_la_LIBADD += $(VORBIS_LIBS)
nobase_libpsynthinc_HEADERS += \
	input/FileReaderOgg.h
libpsynth_la_SOURCES += \
	input/FileReaderOgg.cpp
endif

libpsynth_la_SOURCES += $(nobase_libpsynthinc_HEADERS)

EXTRA_DIST = version.h.in
