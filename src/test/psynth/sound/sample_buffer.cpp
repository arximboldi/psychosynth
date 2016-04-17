/**
 *  Time-stamp:  <2010-11-10 11:44:20 raskolnikov>
 *
 *  @file        sample_buffe.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:58:25 2010
 *
 *  A sample buffer.
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
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

#include <iostream>
#include <cmath>
#include <psynth/sound/stereo.hpp>
#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>
#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/algorithm.hpp>

using namespace psynth::sound;

const size_t sinusoid_size = 1024;

static bits32sf sinusoid [2][sinusoid_size];
stereo32sf_planar_range sample_range = planar_stereo_range (
    sinusoid_size, sinusoid [0], sinusoid [1]);

static struct sinusoid_generator_type
{
    std::size_t _index;

    sinusoid_generator_type ()
	: _index (0)
    {
	generate_frames (sample_range, *this);
    }

    stereo32sf_frame operator () ()
    {
	return stereo32sf_frame (std::sin (_index ++ * M_PI / sinusoid_size));
    }

} sinusoid_generator;
