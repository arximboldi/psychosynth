#
#   PSYCHOSYNTH
#   ===========
#
#   Copyright (C) 2007, 2008, 2010, 2016 by Juan Pedro Bolivar Puente
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

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp")

set(psynth_compile_options
  -DPSYNTH_DATA_DIR="${PSYNTH_DATADIR}")

set(psynth_link_libraries
  ${Boost_LIBRARIES})

if (WITH_ASAN)
  set(psynth_compile_options ${psynth_compile_options}
    "-fno-omit-frame-pointer"
    "-fsanitize=address")
  set(psynth_link_libraries ${psynth_link_libraries}
    "-fsanitize=address")
endif()

set(psynth_include_directories
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/psynth"
  ${Boost_INCLUDE_DIRS})

set(psynth_sources
  app/director.cpp
  app/psynth_app.cpp
  app/file_manager_director.cpp
  base/arg_parser.cpp
  base/misc.cpp
  base/logger.cpp
  base/config.cpp
  base/timer.cpp
  base/file_finder.cpp
  base/file_manager.cpp
  base/exception.cpp
  base/tree.cpp
  base/singleton.cpp
  base/hetero_deque.cpp
  base/factory_manager.cpp
  synth/filter.cpp
  world/world.cpp
  world/patcher.cpp
  world/patcher_dynamic.cpp
  world/world_node_creator.cpp
  graph/buffers.cpp
  graph/node.cpp
  graph/node_param.cpp
  graph/node_factory_manager.cpp
  graph/node_mixer.cpp
  graph/node_manager.cpp
  graph/node_output.cpp
  graph/node_oscillator.cpp
  graph/node_filter.cpp
  graph/node_sampler.cpp
  graph/node_double_sampler.cpp
  graph/node_step_seq.cpp
  graph/node_noise.cpp
  graph/node_echo.cpp
  graph/node_delay.cpp
  io/exception.cpp
  io/async_base.cpp
  io/input.cpp
  io/output.cpp
  io/file_common.cpp
  io/thread_async.cpp
  new_graph/exception.cpp
  new_graph/processor.cpp
  new_graph/node.cpp
  new_graph/sink_node.cpp
  new_graph/process_node.cpp
  new_graph/control.cpp
  new_graph/port.cpp
  new_graph/buffer_port.cpp
  new_graph/soft_buffer_port.cpp
  new_graph/buffers.cpp
  new_graph/core/patch.cpp
  new_graph/core/patch_port.cpp
  new_graph/core/passive_output.cpp
  new_graph/core/async_output.cpp
  new_graph/core/mixer.cpp
  new_graph/core/oscillator.cpp
  new_graph/core/noise.cpp)

set(psynth_headers
  app/director.hpp
  app/defaults.hpp
  app/output_director.hpp
  app/file_manager_director.hpp
  app/psynth_app.hpp
  base/arg_parser.hpp
  base/singleton.hpp
  base/singleton.tpp
  base/iterator.hpp
  base/misc.hpp
  base/logger.hpp
  base/logger_fwd.hpp
  base/config.hpp
  base/config_fwd.hpp
  base/option_conf.hpp
  base/timer.hpp
  base/vector_2d.hpp
  base/file_finder.hpp
  base/file_manager.hpp
  base/tree.hpp
  base/tree.tpp
  base/observer.hpp
  base/pointer.hpp
  base/exception.hpp
  base/throw.hpp
  base/type_value.hpp
  base/type_traits.hpp
  base/threads.hpp
  base/util.hpp
  base/meta.hpp
  base/c3_class.hpp
  base/scope_guard.hpp
  base/compat.hpp
  base/declare.hpp
  base/preprocessor.hpp
  base/concept.hpp
  base/hetero_deque.hpp
  base/hetero_deque.tpp
  base/factory.hpp
  base/factory_manager.hpp
  base/factory_manager.tpp
  base/functor.hpp
  synth/audio_info.hpp
  synth/filter.hpp
  synth/wave_table.hpp
  synth/wave_table.tpp
  synth/oscillator.hpp
  synth/oscillator.tpp
  synth/simple_envelope.hpp
  synth/multi_point_envelope.hpp
  synth/multi_point_envelope.tpp
  synth/envelope.hpp
  synth/util.hpp
  synth/noise.hpp
  synth/noise.tpp
  world/world.hpp
  world/patcher.hpp
  world/patcher_dynamic.hpp
  world/world_node_creator.hpp
  graph/buffers.hpp
  graph/node.hpp
  graph/node_types.hpp
  graph/node_param.hpp
  graph/node_manager.hpp
  graph/node_output.hpp
  graph/node_mixer.hpp
  graph/node_oscillator.hpp
  graph/node_filter.hpp
  graph/node_sampler.hpp
  graph/node_double_sampler.hpp
  graph/node_step_seq.hpp
  graph/node_noise.hpp
  graph/node_echo.hpp
  graph/node_delay.hpp
  graph/watch.hpp
  graph/watch_viewer.hpp
  graph/node_factory.hpp
  graph/node_factory_manager.hpp
  io/exception.hpp
  io/async_base.hpp
  io/buffered_output.hpp
  io/buffered_output.tpp
  io/buffered_input.hpp
  io/buffered_input.tpp
  io/caching_file_input.hpp
  io/caching_file_input.tpp
  io/file_common.hpp
  io/file_common.tpp
  io/input.hpp
  io/input_fwd.hpp
  io/input.tpp
  io/output.hpp
  io/output_fwd.hpp
  io/output.tpp
  io/thread_async.hpp
  sound/algorithm.hpp
  sound/apply_operation_base.hpp
  sound/apply_operation.hpp
  sound/bit_aligned_frame_iterator.hpp
  sound/bit_aligned_frame_reference.hpp
  sound/buffer.hpp
  sound/buffer_range_factory.hpp
  sound/buffer_range.hpp
  sound/channel_base_algorithm.hpp
  sound/channel_base.hpp
  sound/channel_convert.hpp
  sound/concept.hpp
  sound/device_n.hpp
  sound/dynamic_algorithm.hpp
  sound/dynamic_at_c.hpp
  sound/dynamic_buffer.hpp
  sound/dynamic_buffer_range_factory.hpp
  sound/dynamic_buffer_range.hpp
  sound/dynamic_ring_buffer.hpp
  sound/dynamic_ring_buffer_range.hpp
  sound/forwards.hpp
  sound/frame.hpp
  sound/frame_iterator_adaptor.hpp
  sound/frame_iterator.hpp
  sound/metafunctions.hpp
  sound/mono.hpp
  sound/output.hpp
  sound/packed_frame.hpp
  sound/planar_frame_iterator.hpp
  sound/planar_frame_reference.hpp
  sound/quad.hpp
  sound/reduce.hpp
  sound/ring_buffer.hpp
  sound/ring_buffer_range.hpp
  sound/ring_buffer_range.tpp
  sound/sample_algorithm.hpp
  sound/sample.hpp
  sound/step_iterator.hpp
  sound/stereo.hpp
  sound/surround.hpp
  sound/typedefs.hpp
  sound/util.hpp
  sound/variant.hpp
  new_graph/exception.hpp
  new_graph/event.hpp
  new_graph/buffers.hpp
  new_graph/triple_buffer.hpp
  new_graph/triple_buffer.tpp
  new_graph/processor.hpp
  new_graph/processor.tpp
  new_graph/processor_fwd.hpp
  new_graph/node.hpp
  new_graph/node_fwd.hpp
  new_graph/port.hpp
  new_graph/port.tpp
  new_graph/port_fwd.hpp
  new_graph/control.hpp
  new_graph/control.tpp
  new_graph/control_fwd.hpp
  new_graph/buffer_port.hpp
  new_graph/buffer_port_fwd.hpp
  new_graph/soft_buffer_port.hpp
  new_graph/process_node.hpp
  new_graph/process_node_fwd.hpp
  new_graph/sink_node.hpp
  new_graph/sink_node_fwd.hpp
  new_graph/core/patch.hpp
  new_graph/core/patch_fwd.hpp
  new_graph/core/patch_port.hpp
  new_graph/core/async_output.hpp
  new_graph/core/async_output_fwd.hpp
  new_graph/core/passive_output.hpp
  new_graph/core/passive_output_fwd.hpp
  new_graph/core/oscillator.hpp
  new_graph/core/mixer.hpp
  new_graph/core/noise.hpp
  version.hpp)

if (HAVE_SOUNDTOUCH)
  list(APPEND psynth_compile_options ${SOUNDTOUCH_CFLAGS})
  list(APPEND psynth_link_libraries ${SOUNDTOUCH_LDFLAGS})
  list(APPEND psynth_sources
    synth/scaler.cpp)
  list(APPEND psynth_headers
    synth/scaler.hpp
    synth/scaler.tpp)
endif()

if (HAVE_XML)
  list(APPEND psynth_compile_options ${LIBXML_CFLAGS})
  list(APPEND psynth_link_libraries ${LIBXML_LDFLAGS})
  list(APPEND psynth_sources
    base/conf_backend_xml.cpp)
  list(APPEND psynth_headers
    base/conf_backend_xml.hpp
    base/conf_backend_xml_fwd.hpp)
endif()

if (HAVE_OSC)
  list(APPEND psynth_compile_options ${LIBLO_CFLAGS})
  list(APPEND psynth_link_libraries ${LIBLO_LDFLAGS})
  list(APPEND psynth_sources
    net/exception.cpp
    net/osc_broadcast.cpp
    net/osc_controller.cpp
    net/osc_server.cpp
    net/osc_client.cpp
    net/osc_passive.cpp)
  list(APPEND psynth_headers
    net/exception.hpp
    net/osc_broadcast.hpp
    net/osc_server.hpp
    net/osc_client.hpp
    net/osc_passive.hpp
    net/osc_server_logger.hpp
    net/osc_client_logger.hpp
    net/osc_controller.hpp
    net/osc_misc.hpp
    net/osc_protocol.hpp)
endif()

if (HAVE_ALSA)
  list(APPEND psynth_compile_options ${ALSA_CFLAGS})
  list(APPEND psynth_link_libraries ${ALSA_LDFLAGS})
  list(APPEND psynth_sources
    io/alsa_raw_output.cpp)
  list(APPEND psynth_headers
    app/defaults_alsa.hpp
    app/output_director_alsa.hpp
    io/alsa_output.hpp
    io/alsa_output.tpp
    io/alsa_raw_output.hpp)
endif()

if (HAVE_OSS)
  list(APPEND psynth_sources
    io/oss_raw_output.cpp)
  list(APPEND psynth_headers
    app/defaults_oss.hpp
    app/output_director_oss.hpp
    io/oss_output.hpp
    io/oss_output.tpp
    io/oss_raw_output.hpp)
endif()

if (HAVE_JACK)
  list(APPEND psynth_compile_options ${JACK_CFLAGS})
  list(APPEND psynth_link_libraries ${JACK_LDFLAGS})
  list(APPEND psynth_sources
    io/jack_raw_output.cpp)
  list(APPEND psynth_headers
    app/defaults_jack.hpp
    app/output_director_jack.hpp
    io/jack_output.hpp
    io/jack_output.tpp
    io/jack_raw_output.hpp)
endif()

if (HAVE_WAV)
  list(APPEND psynth_compile_options ${SNDFILE_CFLAGS})
  list(APPEND psynth_link_libraries ${SNDFILE_LDFLAGS})
  list(APPEND psynth_sources
    io/file_input.cpp
    io/file_output.cpp)
  list(APPEND psynth_headers
    io/file_input.hpp
    io/file_input.tpp
    io/file_output.hpp
    io/file_output.tpp)
endif()

# SHARED because when it is static, the factory initializers may be
# optimized away if not referenced anywhere
add_library(psynth SHARED ${psynth_sources})
target_include_directories(psynth PUBLIC ${psynth_include_directories})
target_compile_options(psynth PUBLIC ${psynth_compile_options})
target_link_libraries(psynth PUBLIC ${psynth_link_libraries})

install(TARGETS psynth LIBRARY DESTINATION "${PSYNTH_LIBDIR}")
install(FILES ${psynth_headers} DESTINATION "${PSYNTH_INCLUDEDIR}/psynth")
