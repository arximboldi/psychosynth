/**
 *  Time-stamp:  <2011-06-28 17:57:18 raskolnikov>
 *
 *  @file        oscillator.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun 16 20:58:48 2011
 *
 *  @brief Oscillator nodes implementations.
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

#include <iostream>
#include "oscillator.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (audio_sine_oscillator);
PSYNTH_REGISTER_NODE_STATIC (audio_square_oscillator);
PSYNTH_REGISTER_NODE_STATIC (audio_triangle_oscillator);
PSYNTH_REGISTER_NODE_STATIC (audio_sawtooth_oscillator);
PSYNTH_REGISTER_NODE_STATIC (audio_moogsaw_oscillator);
PSYNTH_REGISTER_NODE_STATIC (audio_exp_oscillator);

PSYNTH_REGISTER_NODE_STATIC (sample_sine_oscillator);
PSYNTH_REGISTER_NODE_STATIC (sample_square_oscillator);
PSYNTH_REGISTER_NODE_STATIC (sample_triangle_oscillator);
PSYNTH_REGISTER_NODE_STATIC (sample_sawtooth_oscillator);
PSYNTH_REGISTER_NODE_STATIC (sample_moogsaw_oscillator);
PSYNTH_REGISTER_NODE_STATIC (sample_exp_oscillator);

constexpr float default_frequency = 440.0f;
constexpr float default_amplitude = 0.5f;
constexpr int   default_modulator = 1;

template <class G, class O>
oscillator<G, O>::oscillator ()
    : _out_output ("output", this)
    , _in_modulator ("modulator", this, 1.0f)
    , _ctl_frequency ("frequency", this, default_frequency)
    , _ctl_amplitude ("amplitude", this, default_amplitude)
    , _ctl_modulator ("modulator", this, default_modulator)
    , _osc (44100.0f,            // Doesn't matter
            default_frequency,
            default_amplitude)
{
}

template <class G, class O>
void oscillator<G, O>::rt_on_context_update (rt_process_context& ctx)
{
    _osc.set_frame_rate (ctx.frame_rate ());
}

template <class G, class O>
void oscillator<G, O>::rt_do_process (rt_process_context& ctx)
{
    _osc.set_frequency (_ctl_frequency.rt_get ());
    _osc.set_amplitude (_ctl_amplitude.rt_get ());

    if (!_in_modulator.rt_in_available ())
        _osc.update (_out_output.rt_out_range ());
    else
    {
        switch (_ctl_modulator.rt_get ())
        {
        case 0:
            _osc.update_am (_out_output.rt_out_range (),
                            _in_modulator.rt_in_range ());
            break;
        case 1:
            _osc.update_fm (_out_output.rt_out_range (),
                            _in_modulator.rt_in_range ());
            break;
        case 2:
            _osc.update_pm (_out_output.rt_out_range (),
                            _in_modulator.rt_in_range ());
            break;
        default:
            _osc.update (_out_output.rt_out_range ());
        }
    }
}

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
