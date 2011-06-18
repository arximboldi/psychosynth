/**
 *  Time-stamp:  <2011-06-18 13:22:37 raskolnikov>
 *
 *  @file        mixer.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 17 18:56:22 2011
 *
 *  @brief Mixer node implementation.
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
#include <boost/lexical_cast.hpp>
#include "sound/output.hpp"
#include "mixer.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (audio_mixer);
PSYNTH_REGISTER_NODE_STATIC (sample_mixer);

constexpr float default_gain = 0.5f;
constexpr int default_inputs = 16;

template <class B>
mixer<B>::mixer ()
    : _in_modulator ("modulator", sample_frame (1.0f), this)
    , _out_output ("output", this)
    , _ctl_gain ("gain", default_gain, this)
    , _ctl_inputs ("inputs", default_inputs, this)
{
    // FIXME: We are using shared_ptr because for some reason
    // generate_n requires copying. Take a deeper look at it and
    // realise wether it is a GCC bug.

    std::size_t iter = 0;
    std::generate_n (
        std::back_inserter (_in_inputs),
        default_inputs,
        [&] () -> in_port_ptr {
            return std::make_shared<in_port_type> (
                std::string ("input-") +
                boost::lexical_cast<std::string> (iter ++), this);
        });
}

template <class B>
void mixer<B>::rt_do_process (rt_process_context& ctx)
{
    typedef typename in_port_type::port_type::value_type frame_type;
    typedef typename sound::sample_type<frame_type>::type sample_type;
            
    int num_mixed = 0;

    auto out  = _out_output.rt_out_range ();
    float gain = _ctl_gain.rt_get ();

    sound::fill_frames (out, frame_type (1.0f));    
    for (auto& in : _in_inputs)
    {
        if (in->rt_in_available ())
        {
            sound::transform_frames (
                in->rt_in_range (), out, out,
                [&] (const frame_type& a, const frame_type& b) {
                    frame_type res;
                    // FIXME: This multiplication can be done with the
                    // numeric algorithms for safety with non float types.
                    static_transform (a, b, res,
                                      [&] (sample_type a, sample_type b)
                                      { return a * b * gain; });
                    return res;
                }); 
            ++ num_mixed;
        }
    }
            
    if (num_mixed == 0)
    {
        auto zero = sound::sample_traits<sample_type>::zero_value ();
        sound::fill_frames (out, frame_type (zero));
    }
}

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
