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

AM_CXXFLAGS = -Wall $(BOOST_CPPFLAGS)
AM_CPPFLAGS = 
AM_LDFLAGS  = $(BOOST_LDFLAGS)

#  Libraries to build
# ======================================================================

lib_LTLIBRARIES = libpsynth.la

#  libpsynth compilation
# ======================================================================

libpsynth_la_CXXFLAGS =
libpsynth_la_LDFLAGS =  
libpsynth_la_LIBADD = $(BOOST_THREAD_LIB)

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
	world/world.h \
	world/patcher.h\
	world/patcher_dynamic.h \
	world/world_node_creator.h\
	node/node.h \
	node/node_manager.h \
	node/node_output.h \
	node/node_mixer.h \
	node/node_audio_mixer.h \
	node/node_control_mixer.h \
	node/node_oscillator.h \
	node/node_lfo.h \
	node/node_audio_oscillator.h \
	node/node_filter.h \
	node/node_sampler.h \
	node/node_step_seq.h \
	node/node_noise.h \
	node/node_audio_noise.h \
	node/node_control_noise.h \
	node/node_echo.h \
	node/node_delay.h \
	node/filter.h \
	node/wave_table.h \
	node/oscillator.h \
	node/node_types.h \
	node/node_param.h\
	node/envelope_simple.h\
	node/envelope_multi.h\
	node/envelope.h\
	node/watch.h\
	node/watch_viewer.h\
	node/node_factory.h\
	node/node_factory_manager.h\
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
	common/misc.cpp \
	common/logger.cpp \
	common/config.cpp \
	common/timer.cpp \
	common/file_finder.cpp\
	common/file_manager.cpp\
	world/world.cpp \
	world/patcher.cpp \
	world/patcher_dynamic.cpp \
	world/world_node_creator.cpp\
	node/node.cpp \
	node/node_mixer.cpp \
	node/node_audio_mixer.cpp \
	node/node_control_mixer.cpp \
	node/node_manager.cpp \
	node/node_output.cpp \
	node/node_oscillator.cpp \
	node/node_lfo.cpp \
	node/node_audio_oscillator.cpp \
	node/node_filter.cpp \
	node/node_sampler.cpp \
	node/node_step_seq.cpp \
	node/node_noise.cpp \
	node/node_audio_noise.cpp \
	node/node_control_noise.cpp \
	node/node_echo.cpp \
	node/node_delay.cpp \
	node/filter.cpp \
	node/wave_table.cpp \
	node/oscillator.cpp\
	node/node_param.cpp \
	node/node_factory_manager.cpp\
	node/envelope_multi.cpp\
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
	net/osc_broadcast.h\
	net/osc_server.h \
	net/osc_client.h \
	net/osc_server_logger.h \
	net/osc_client_logger.h \
	net/osc_controller.h \
	net/osc_misc.h \
	net/osc_protocol.h
libpsynth_la_SOURCES += \
	net/osc_broadcast.cpp \
	net/osc_controller.cpp \
	net/osc_server.cpp \
	net/osc_client.cpp
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
