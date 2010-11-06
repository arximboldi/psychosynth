/**
 *  Time-stamp:  <2010-11-05 13:42:24 raskolnikov>
 *
 *  @file        sample_buffe.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:58:25 2010
 *
 *  A sample buffer.
 */

#include <cmath>
#include <psynth/sound/stereo.hpp>
#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>
#include <psynth/sound/buffer_view.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/algorithm.hpp>

using namespace psynth::sound;

const size_t sinusoid_size = 1024;

static bits32sf sinusoid [2][sinusoid_size];
stereo32sf_planar_view sample_view = planar_stereo_view (
    sinusoid_size, sinusoid [0], sinusoid [1]);

static struct sinusoid_generator_type
{
    std::size_t _index;

    sinusoid_generator_type ()
	: _index (0)
    {
	generate_frames (sample_view, *this);
    }

    stereo32sf_frame operator () ()
    {
	return stereo32sf_frame (std::sin (_index ++ * M_PI / sinusoid_size));
    }
    
} sinusoid_generator;
