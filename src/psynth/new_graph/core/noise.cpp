/*
 *  File:       noise.cpp
 *  Author:     Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  Date:       Sun Feb  5 00:46:05 2012
 *  Time-stamp: <2012-02-05 08:56:18 raskolnikov>
 *
 *  Noise generation node implementation.
 */

/*
 *  Copyright (C) 2012 Juan Pedro Bolívar Puente
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

#include <psynth/sound/buffer_range_factory.hpp>
#include "noise.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (audio_white_noise);
PSYNTH_REGISTER_NODE_STATIC (audio_pink_noise);
PSYNTH_REGISTER_NODE_STATIC (sample_white_noise);
PSYNTH_REGISTER_NODE_STATIC (sample_pink_noise);

template <template<class> class D, class O>
noise<D, O>::noise ()
    : _out_output ("output", this)
    , _in_modulator ("modulator", this, 1.0f)
    , _ctl_amplitude ("amplitude", this, 1.0f)
{
}

template <template<class> class D, class Output>
void noise<D, Output>::rt_do_process (rt_process_context& ctx)
{
    typedef typename Output::port_type::value_type out_frame_type;
    auto out_buf = _out_output.rt_out_range ();
    auto mod_buf = sound::channel_converted_range<out_frame_type>(
        _in_modulator.rt_in_range ());
    _noise.update (out_buf);
    synth::modulate (out_buf, mod_buf, _ctl_amplitude.rt_get (), out_buf);
}


} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
