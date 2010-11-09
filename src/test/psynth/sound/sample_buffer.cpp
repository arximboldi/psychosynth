/**
 *  Time-stamp:  <2010-11-09 03:08:25 raskolnikov>
 *
 *  @file        sample_buffe.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:58:25 2010
 *
 *  A sample buffer.
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
