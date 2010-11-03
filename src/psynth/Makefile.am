#
#   PSYCHOSYNTH
#   ===========
#              
#   Copyright (C) 2007, 2008, 2010 by Juan Pedro Bolivar Puente
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

AM_CPPFLAGS = -Wall $(BOOST_CPPFLAGS)
AM_LDFLAGS  = $(BOOST_LDFLAGS)

#  Libraries to build
# ======================================================================

lib_LTLIBRARIES = libpsynth.la

#  libpsynth compilation
# ======================================================================

libpsynth_la_CXXFLAGS = $(SIGC_CFLAGS) -I.. \
	-DPSYNTH_DATA_DIR=\"$(PACKAGE_DATA_DIR)\"
libpsynth_la_LDFLAGS =  
libpsynth_la_LIBADD = $(BOOST_THREAD_LIB) $(BOOST_FILESYSTEM_LIB) $(SIGC_LIBS)

libpsynthincdir = $(PACKAGE_INCLUDE_DIR)

nobase_libpsynthinc_HEADERS = \
	app/director.hpp \
	app/defaults.hpp \
	app/output_director.hpp \
	app/file_manager_director.hpp \
	app/psynth_app.hpp \
	base/arg_parser.hpp \
	base/singleton.hpp \
	base/singleton.tpp \
        base/iterator.hpp \
	base/misc.hpp \
	base/logger.hpp \
	base/logger_def.hpp \
	base/config.hpp \
	base/config_def.hpp \
	base/option_conf.hpp \
	base/timer.hpp \
	base/vector_2d.hpp\
	base/file_finder.hpp\
	base/file_manager.hpp\
	base/tree.hpp\
	base/tree.tpp\
	base/observer.hpp\
	base/pointer.hpp\
	base/exception.hpp\
	base/type_value.hpp\
	base/threads.hpp\
	base/util.hpp\
	synth/scaler.hpp\
	synth/audio_buffer.hpp \
	synth/sample_buffer.hpp\
	synth/ring_buffer.hpp\
	synth/ring_audio_buffer.hpp \
	synth/ring_sample_buffer.hpp \
	synth/audio_info.hpp \
	synth/filter.hpp \
	synth/wave_table.hpp \
	synth/oscillator.hpp \
	synth/envelope_simple.hpp\
	synth/envelope_multi.hpp\
	synth/envelope.hpp\
	world/world.hpp \
	world/patcher.hpp\
	world/patcher_dynamic.hpp \
	world/world_node_creator.hpp\
	node/node.hpp \
	node/node_types.hpp \
	node/node_param.hpp\
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
	node/watch.hpp\
	node/watch_viewer.hpp\
	node/node_factory.hpp\
	node/node_factory_manager.hpp\
	io/output.hpp\
	io/file_reader.hpp\
	io/file_reader_any.hpp\
	io/file_reader_fetcher.hpp\
	version.hpp

libpsynth_la_SOURCES = \
	app/director.cpp \
	app/psynth_app.cpp \
	app/file_manager_director.cpp \
	base/arg_parser.cpp \
	base/misc.cpp \
	base/logger.cpp \
	base/config.cpp \
	base/timer.cpp \
	base/file_finder.cpp\
	base/file_manager.cpp\
	base/exception.cpp\
	base/tree.cpp\
	base/singleton.cpp\
	synth/audio_buffer.cpp \
	synth/sample_buffer.cpp \
	synth/ring_buffer.cpp \
	synth/ring_audio_buffer.cpp \
	synth/ring_sample_buffer.cpp \
	synth/filter.cpp \
	synth/wave_table.cpp \
	synth/oscillator.cpp\
	synth/envelope_multi.cpp\
	world/world.cpp \
	world/patcher.cpp \
	world/patcher_dynamic.cpp \
	world/world_node_creator.cpp\
	node/node.cpp \
	node/node_param.cpp \
	node/node_factory_manager.cpp\
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
	io/file_reader_any.cpp\
	io/file_reader_fetcher.cpp

if HAVE_SAMPLERATE
libpsynth_la_CXXFLAGS += $(SAMPLERATE_CFLAGS)
libpsynth_la_LIBADD += $(SAMPLERATE_LIBS)
libpsynth_la_SOURCES += \
	synth/scaler_src.cpp
nobase_libpsynthinc_HEADERS += \
	synth/scaler_src.hpp
endif

if HAVE_SOUNDTOUCH
libpsynth_la_CXXFLAGS += $(SOUNDTOUCH_CFLAGS)
libpsynth_la_LIBADD += $(SOUNDTOUCH_LIBS)
libpsynth_la_SOURCES += \
	synth/scaler_st.cpp
nobase_libpsynthinc_HEADERS += \
	synth/scaler_st.hpp
endif

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	base/conf_backend_xml.cpp
nobase_libpsynthinc_HEADERS += \
	base/conf_backend_xml.hpp \
	base/conf_backend_xml_def.hpp
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
	app/defaults_alsa.hpp \
	app/output_director_alsa.hpp \
	io/output_alsa.hpp	
libpsynth_la_SOURCES += \
	io/output_alsa.cpp
endif

if HAVE_OSS
nobase_libpsynthinc_HEADERS += \
	app/defaults_oss.hpp \
	app/output_director_oss.hpp \
	io/output_oss.hpp
libpsynth_la_SOURCES += \
	io/output_oss.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynthinc_HEADERS += \
	app/defaults_jack.hpp \
	app/output_director_jack.hpp \
	io/output_jack.hpp
libpsynth_la_SOURCES += \
	io/output_jack.cpp
endif

if HAVE_SNDFILE
libpsynth_la_CXXFLAGS += $(SNDFILE_CFLAGS)
libpsynth_la_LIBADD += $(SNDFILE_LIBS)
nobase_libpsynthinc_HEADERS += \
	io/output_wave.hpp\
	io/file_reader_wave.hpp
libpsynth_la_SOURCES += \
	io/output_wave.cpp\
	io/file_reader_wave.cpp
endif

if HAVE_VORBIS
libpsynth_la_CXXFLAGS += $(VORBIS_CFLAGS)
libpsynth_la_LIBADD += $(VORBIS_LIBS)
nobase_libpsynthinc_HEADERS += \
	io/file_reader_ogg.hpp
libpsynth_la_SOURCES += \
	io/file_reader_ogg.cpp
endif

libpsynth_la_SOURCES += $(nobase_libpsynthinc_HEADERS)

EXTRA_DIST = version.hpp.in
