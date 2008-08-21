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
	psynth/director.h \
	psynth/defaults.h \
	psynth/output_director.h \
	psynth/file_manager_director.h \
	psynth/psynth_app.h \
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
	output/output.h\
	input/file_reader.h\
	input/file_reader_any.h\
	input/file_reader_fetcher.h\
	version.h

libpsynth_la_SOURCES = \
	psynth/director.cpp \
	psynth/psynth_app.cpp \
	psynth/file_manager_director.cpp \
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
	input/file_reader_any.cpp\
	input/file_reader_fetcher.cpp

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
	psynth/defaults_alsa.h \
	psynth/output_director_alsa.h \
	output/output_alsa.h	
libpsynth_la_SOURCES += \
	output/output_alsa.cpp
endif

if HAVE_OSS
nobase_libpsynthinc_HEADERS += \
	psynth/defaults_oss.h \
	psynth/output_director_oss.h \
	output/output_oss.h
libpsynth_la_SOURCES += \
	output/output_oss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynthinc_HEADERS += \
	psynth/defaults_jack.h \
	psynth/output_director_jack.h \
	output/output_jack.h
libpsynth_la_SOURCES += \
	output/output_jack.cpp
endif

if HAVE_SNDFILE
libpsynth_la_CXXFLAGS += $(SNDFILE_CFLAGS)
libpsynth_la_LIBADD += $(SNDFILE_LIBS)
nobase_libpsynthinc_HEADERS += \
	output/output_wave.h\
	input/file_reader_wave.h
libpsynth_la_SOURCES += \
	output/output_wave.cpp\
	input/file_reader_wave.cpp
endif

if HAVE_VORBIS
libpsynth_la_CXXFLAGS += $(VORBIS_CFLAGS)
libpsynth_la_LIBADD += $(VORBIS_LIBS)
nobase_libpsynthinc_HEADERS += \
	input/file_reader_ogg.h
libpsynth_la_SOURCES += \
	input/file_reader_ogg.cpp
endif

libpsynth_la_SOURCES += $(nobase_libpsynthinc_HEADERS)

EXTRA_DIST = version.h.in
