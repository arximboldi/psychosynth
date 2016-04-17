/**
 *  Time-stamp:  <2011-03-18 23:00:14 raskolnikov>
 *
 *  @file        buffers.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 11:29:59 2011
 *
 *  @brief Default buffers for the node layer.
 *  @todo  Add extern templates.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_GRAPH_BUFFERS_H_
#define PSYNTH_GRAPH_BUFFERS_H_

/**
 * @todo Remove all these no typedef includes and make them optional
 * later for compile time performance optimization.
 */

#include <psynth/sound/buffer.hpp>
#include <psynth/sound/ring_buffer.hpp>
#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/ring_buffer_range.hpp>

#include <psynth/sound/typedefs.hpp>

namespace psynth
{
namespace graph
{

typedef sound::stereo32sf_planar_buffer            audio_buffer;
typedef sound::stereo32sf_planar_range             audio_range;
typedef sound::stereo32sfc_planar_range            audio_const_range;
typedef sound::stereo32sf_planar_ring_buffer       audio_ring_buffer;
typedef sound::stereo32sf_planar_ring_range        audio_ring_range;
typedef audio_range::value_type                    audio_frame;
typedef sound::bits32sf                            audio_sample;

typedef sound::mono32sf_buffer                     sample_buffer;
typedef sound::mono32sf_range                      sample_range;
typedef sound::mono32sfc_range                     sample_const_range;
typedef sound::mono32sf_ring_buffer                sample_ring_buffer;
typedef sound::mono32sf_ring_range                 sample_ring_range;
typedef typename sample_range::value_type          sample_frame;
typedef sound::bits32sf                            sample_sample;

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_BUFFERS_H_ */
