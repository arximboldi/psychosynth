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

libpsynth_la_CXXFLAGS = $(SIGC_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	-DPSYNTH_DATA_DIR=\"$(PACKAGE_DATA_DIR)\"
libpsynth_la_LDFLAGS =  
libpsynth_la_LIBADD = \
	$(BOOST_FILESYSTEM_LIB) \
	$(BOOST_SIGNALS_LIB) \
	-lboost_system-mt

libpsynthincdir = $(PACKAGE_INCLUDE_DIR)


nobase_libpsynthinc_HEADERS = \
	app/director.hpp\
	app/defaults.hpp\
	app/output_director.hpp\
	app/file_manager_director.hpp\
	app/psynth_app.hpp\
	base/arg_parser.hpp\
	base/singleton.hpp\
	base/singleton.tpp\
        base/iterator.hpp\
	base/misc.hpp\
	base/logger.hpp\
	base/logger_fwd.hpp\
	base/config.hpp\
	base/config_fwd.hpp\
	base/option_conf.hpp\
	base/timer.hpp\
	base/vector_2d.hpp\
	base/file_finder.hpp\
	base/file_manager.hpp\
	base/tree.hpp\
	base/tree.tpp\
	base/observer.hpp\
	base/pointer.hpp\
	base/exception.hpp\
	base/throw.hpp\
	base/type_value.hpp\
	base/threads.hpp\
	base/util.hpp\
	base/meta.hpp\
	base/c3_class.hpp\
	base/scope_guard.hpp\
	base/compat.hpp\
	base/declare.hpp\
	base/preprocessor.hpp\
	base/concept.hpp\
	base/hetero_deque.hpp\
	base/hetero_deque.tpp\
	base/factory.hpp\
	base/factory_manager.hpp\
	base/factory_manager.tpp\
	synth/audio_info.hpp\
	synth/filter.hpp\
	synth/wave_table.hpp\
	synth/wave_table.tpp\
	synth/oscillator.hpp\
	synth/oscillator.tpp\
	synth/simple_envelope.hpp\
	synth/multi_point_envelope.hpp\
	synth/multi_point_envelope.tpp\
	synth/envelope.hpp\
	synth/util.hpp\
	world/world.hpp\
	world/patcher.hpp\
	world/patcher_dynamic.hpp\
	world/world_node_creator.hpp\
	graph/buffers.hpp\
	graph/node.hpp\
	graph/node_types.hpp\
	graph/node_param.hpp\
	graph/node_manager.hpp\
	graph/node_output.hpp\
	graph/node_mixer.hpp\
	graph/node_oscillator.hpp\
	graph/node_filter.hpp\
	graph/node_sampler.hpp\
	graph/node_double_sampler.hpp\
	graph/node_step_seq.hpp\
	graph/node_noise.hpp\
	graph/node_echo.hpp\
	graph/node_delay.hpp\
	graph/watch.hpp\
	graph/watch_viewer.hpp\
	graph/node_factory.hpp\
	graph/node_factory_manager.hpp\
	io/exception.hpp\
	io/async_base.hpp\
	io/buffered_output.hpp\
	io/buffered_output.tpp\
	io/buffered_input.hpp\
	io/buffered_input.tpp\
	io/caching_file_input.hpp\
	io/caching_file_input.tpp\
	io/file_common.hpp\
	io/file_common.tpp\
	io/input.hpp\
	io/input_fwd.hpp\
	io/input.tpp\
	io/output.hpp\
	io/output_fwd.hpp\
	io/output.tpp\
	io/thread_async.hpp\
	sound/algorithm.hpp\
	sound/apply_operation_base.hpp\
	sound/apply_operation.hpp\
	sound/bit_aligned_frame_iterator.hpp\
	sound/bit_aligned_frame_reference.hpp\
	sound/buffer.hpp\
	sound/buffer_range_factory.hpp\
	sound/buffer_range.hpp\
	sound/channel_base_algorithm.hpp\
	sound/channel_base.hpp\
	sound/channel_convert.hpp\
	sound/concept.hpp\
	sound/device_n.hpp\
	sound/dynamic_algorithm.hpp\
	sound/dynamic_at_c.hpp\
	sound/dynamic_buffer.hpp\
	sound/dynamic_buffer_range_factory.hpp\
	sound/dynamic_buffer_range.hpp\
	sound/dynamic_ring_buffer.hpp\
	sound/dynamic_ring_buffer_range.hpp\
	sound/forwards.hpp\
	sound/frame.hpp\
	sound/frame_iterator_adaptor.hpp\
	sound/frame_iterator.hpp\
	sound/metafunctions.hpp\
	sound/mono.hpp\
	sound/output.hpp\
	sound/packed_frame.hpp\
	sound/planar_frame_iterator.hpp\
	sound/planar_frame_reference.hpp\
	sound/quad.hpp\
	sound/reduce.hpp\
	sound/ring_buffer.hpp\
	sound/ring_buffer_range.hpp\
	sound/ring_buffer_range.tpp\
	sound/sample_algorithm.hpp\
	sound/sample.hpp\
	sound/step_iterator.hpp\
	sound/stereo.hpp\
	sound/surround.hpp\
	sound/typedefs.hpp\
	sound/util.hpp\
	sound/variant.hpp\
	version.hpp

libpsynth_la_SOURCES = \
	app/director.cpp\
	app/psynth_app.cpp\
	app/file_manager_director.cpp\
	base/arg_parser.cpp\
	base/misc.cpp\
	base/logger.cpp\
	base/config.cpp\
	base/timer.cpp\
	base/file_finder.cpp\
	base/file_manager.cpp\
	base/exception.cpp\
	base/tree.cpp\
	base/singleton.cpp\
	base/hetero_deque.cpp\
	base/factory_manager.cpp\
	synth/filter.cpp\
	world/world.cpp\
	world/patcher.cpp\
	world/patcher_dynamic.cpp\
	world/world_node_creator.cpp\
	graph/buffers.cpp\
	graph/node.cpp\
	graph/node_param.cpp\
	graph/node_factory_manager.cpp\
	graph/node_mixer.cpp\
	graph/node_manager.cpp\
	graph/node_output.cpp\
	graph/node_oscillator.cpp\
	graph/node_filter.cpp\
	graph/node_sampler.cpp\
	graph/node_double_sampler.cpp\
	graph/node_step_seq.cpp\
	graph/node_noise.cpp\
	graph/node_echo.cpp\
	graph/node_delay.cpp\
	io/exception.cpp\
	io/async_base.cpp\
	io/input.cpp\
	io/output.cpp\
	io/file_common.cpp\
	io/thread_async.cpp

if HAVE_SOUNDTOUCH
libpsynth_la_CXXFLAGS += $(SOUNDTOUCH_CFLAGS)
libpsynth_la_LIBADD += $(SOUNDTOUCH_LIBS)
libpsynth_la_SOURCES += \
	synth/scaler.cpp
nobase_libpsynthinc_HEADERS += \
	synth/scaler.hpp \
	synth/scaler.tpp
endif

if HAVE_LIBXML
libpsynth_la_CXXFLAGS += $(LIBXML_CFLAGS)
libpsynth_la_LIBADD += $(LIBXML_LIBS)
libpsynth_la_SOURCES += \
	base/conf_backend_xml.cpp
nobase_libpsynthinc_HEADERS += \
	base/conf_backend_xml.hpp\
	base/conf_backend_xml_fwd.hpp
endif

if HAVE_LIBLO
libpsynth_la_CXXFLAGS += $(LIBLO_CFLAGS)
libpsynth_la_LIBADD += $(LIBLO_LIBS)
nobase_libpsynthinc_HEADERS += \
	net/osc_broadcast.hpp\
	net/osc_server.hpp\
	net/osc_client.hpp\
	net/osc_server_logger.hpp\
	net/osc_client_logger.hpp\
	net/osc_controller.hpp\
	net/osc_misc.hpp\
	net/osc_protocol.hpp
libpsynth_la_SOURCES += \
	net/osc_broadcast.cpp\
	net/osc_controller.cpp\
	net/osc_server.cpp\
	net/osc_client.cpp
endif

if HAVE_ALSA
libpsynth_la_CXXFLAGS += $(ALSA_CFLAGS)
libpsynth_la_LIBADD += $(ALSA_LIBS)
nobase_libpsynthinc_HEADERS += \
	app/defaults_alsa.hpp\
	app/output_director_alsa.hpp\
	io/alsa_output.hpp\
	io/alsa_output.tpp\
	io/alsa_raw_output.hpp
libpsynth_la_SOURCES += \
	io/alsa_raw_output.cpp
endif

if HAVE_OSS
nobase_libpsynthinc_HEADERS += \
	app/defaults_oss.hpp\
	app/output_director_oss.hpp\
	io/oss_output.hpp\
	io/oss_output.tpp\
	io/oss_raw_output.hpp
libpsynth_la_SOURCES += \
	io/oss_raw_output.cpp
endif

if HAVE_JACK
libpsynth_la_CXXFLAGS += $(JACK_CFLAGS)
libpsynth_la_LDFLAGS += $(JACK_LIBS)
nobase_libpsynthinc_HEADERS += \
	app/defaults_jack.hpp\
	app/output_director_jack.hpp\
	io/jack_output.hpp\
	io/jack_output.tpp\
	io/jack_raw_output.hpp
libpsynth_la_SOURCES += \
	io/jack_raw_output.cpp
endif

if HAVE_SNDFILE
libpsynth_la_CXXFLAGS += $(SNDFILE_CFLAGS)
libpsynth_la_LIBADD += $(SNDFILE_LIBS)
nobase_libpsynthinc_HEADERS += \
	io/file_input.hpp\
	io/file_input.tpp\
	io/file_output.hpp\
	io/file_output.tpp
libpsynth_la_SOURCES += \
	io/file_input.cpp\
	io/file_output.cpp
endif

libpsynth_la_SOURCES += $(nobase_libpsynthinc_HEADERS)

EXTRA_DIST = version.hpp.in
