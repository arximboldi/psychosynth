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

libpsynth_la_CXXFLAGS = $(SIGC_CFLAGS)
libpsynth_la_LDFLAGS =  
libpsynth_la_LIBADD = $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) $(SIGC_LIBS)

libpsynthincdir = @PACKAGE_INCLUDE_DIR@

nobase_libpsynthinc_HEADERS = \
	psynth/director.hpp \
	psynth/defaults.hpp \
	psynth/output_director.hpp \
	psynth/file_manager_director.hpp \
	psynth/psynth_app.hpp \
	common/arg_parser.hpp \
	common/deleter.hpp \
	common/error.hpp \
	common/singleton.hpp \
	common/audio_buffer.hpp \
	common/sample_buffer.hpp\
	common/ring_buffer.hpp\
	common/ring_audio_buffer.hpp \
	common/ring_sample_buffer.hpp \
	common/audio_info.hpp \
        common/map_iterator.hpp \
	common/misc.hpp \
	common/logger.hpp \
	common/config.hpp \
	common/option_conf.hpp \
	common/timer.hpp \
	common/vector_2d.hpp\
	common/scaler.hpp\
	common/file_finder.hpp\
	common/file_manager.hpp\
	common/tree.hpp\
	common/signal.hpp\
	common/memory.hpp\
	world/world.hpp \
	world/patcher.hpp\
	world/patcher_dynamic.hpp \
	world/world_node_creator.hpp\
	node/node.hpp \
	node/node_manager.hpp \
	node/node_output.hpp \
	node/node_mixer.hpp \
	node/node_audio_mixer.hpp \
	node/node_control_mixer.hpp \
	node/node_oscillator.hpp \
	node/node_lfo.hpp \
	node/node_audio_oscillator.hpp \
	node/node_filter.hpp \
	node/node_sampler.hpp \
	node/node_double_sampler.hpp \
	node/node_step_seq.hpp \
	node/node_noise.hpp \
	node/node_audio_noise.hpp \
	node/node_control_noise.hpp \
	node/node_echo.hpp \
	node/node_delay.hpp \
	node/filter.hpp \
	node/wave_table.hpp \
	node/oscillator.hpp \
	node/node_types.hpp \
	node/node_param.hpp\
	node/envelope_simple.hpp\
	node/envelope_multi.hpp\
	node/envelope.hpp\
	node/watch.hpp\
	node/watch_viewer.hpp\
	node/node_factory.hpp\
	node/node_factory_manager.hpp\
	output/output.hpp\
	input/file_reader.hpp\
	input/file_reader_any.hpp\
	input/file_reader_fetcher.hpp\
	version.hpp

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
	node/node_double_sampler.cpp \
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
	common/scaler_src.hpp
endif

if HAVE_SOUNDTOUCH
libpsynth_la_CXXFLAGS += $(SOUNDTOUCH_CFLAGS)
libpsynth_la_LIBADD += $(SOUNDTOUCH_LIBS)
libpsynth_la_SOURCES += \
	common/scaler_st.cpp
nobase_libpsynthinc_HEADERS += \
	common/scaler_st.hpp
endif

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	common/conf_backend_xml.cpp
nobase_libpsynthinc_HEADERS += \
	common/conf_backend_xml.hpp
endif

if HAVE_LIBLO
libpsynth_la_CXXFLAGS += $(LIBLO_CFLAGS)
libpsynth_la_LIBADD += $(LIBLO_LIBS)
nobase_libpsynthinc_HEADERS += \
	net/osc_broadcast.hpp\
	net/osc_server.hpp \
	net/osc_client.hpp \
	net/osc_server_logger.hpp \
	net/osc_client_logger.hpp \
	net/osc_controller.hpp \
	net/osc_misc.hpp \
	net/osc_protocol.hpp
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
	psynth/defaults_alsa.hpp \
	psynth/output_director_alsa.hpp \
	output/output_alsa.hpp	
libpsynth_la_SOURCES += \
	output/output_alsa.cpp
endif

if HAVE_OSS
nobase_libpsynthinc_HEADERS += \
	psynth/defaults_oss.hpp \
	psynth/output_director_oss.hpp \
	output/output_oss.hpp
libpsynth_la_SOURCES += \
	output/output_oss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynthinc_HEADERS += \
	psynth/defaults_jack.hpp \
	psynth/output_director_jack.hpp \
	output/output_jack.hpp
libpsynth_la_SOURCES += \
	output/output_jack.cpp
endif

if HAVE_SNDFILE
libpsynth_la_CXXFLAGS += $(SNDFILE_CFLAGS)
libpsynth_la_LIBADD += $(SNDFILE_LIBS)
nobase_libpsynthinc_HEADERS += \
	output/output_wave.hpp\
	input/file_reader_wave.hpp
libpsynth_la_SOURCES += \
	output/output_wave.cpp\
	input/file_reader_wave.cpp
endif

if HAVE_VORBIS
libpsynth_la_CXXFLAGS += $(VORBIS_CFLAGS)
libpsynth_la_LIBADD += $(VORBIS_LIBS)
nobase_libpsynthinc_HEADERS += \
	input/file_reader_ogg.hpp
libpsynth_la_SOURCES += \
	input/file_reader_ogg.cpp
endif

libpsynth_la_SOURCES += $(nobase_libpsynthinc_HEADERS)

EXTRA_DIST = version.hpp.in
