/**
 *  Time-stamp:  <2011-06-18 21:15:01 raskolnikov>
 *
 *  @file        simple_envelope.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 23:04:45 2011
 *
 *  @brief Very minimalistic envelope.
 *  @todo Check that we do not break anything by changing float sample
 *  to std::size_t sample in the update () parameter.
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

#ifndef PSYNTH_SYNTH_SIMPLE_ENVELOPE_H
#define PSYNTH_SYNTH_SIMPLE_ENVELOPE_H

#include <psynth/sound/forwards.hpp>
#include <psynth/sound/algorithm.hpp>
#include <psynth/synth/envelope.hpp>

namespace psynth
{
namespace synth
{

/**
 * Simplistic evenlope implementation with only two points.
 */
template <class Range>
class simple_envelope : public envelope<Range>
{
public:
    typedef Range range;
    typedef typename range::value_type               value_type;
    typedef typename sound::sample_type<range>::type sample_type;

    simple_envelope ()
	: _rise_dt (0.0f)
	, _fall_dt (0.0f)
	, _curr_dt (0.0f)
	, _val (0.0f)
	{}
    
    simple_envelope (float rise_dt, float fall_dt)
	: _rise_dt (rise_dt)
	, _fall_dt (fall_dt)
	, _curr_dt (0.0f)
	, _val (0.0f)
	{}

    void set_deltas (float rise_dt, float fall_dt)
    {
	_rise_dt = rise_dt;
	_fall_dt = fall_dt;
    }

    void set (sample_type value)
    { _val = value; }
    
    value_type update ()
    {
        const sample_type one  =
            sound::sample_traits<sample_type>::max_value ();
        const sample_type zero =
            sound::sample_traits<sample_type>::zero_value ();
        sample_type val = _val;
        
	_val = _val + _curr_dt;
	if (_val > one)
            _val = one;
	else if (_val < zero)
            _val = zero;
	return value_type { val };
    }

    value_type update (std::size_t sample)
    {
        const sample_type one  =
            sound::sample_traits<sample_type>::max_value ();
        const sample_type zero =
            sound::sample_traits<sample_type>::zero_value ();
        sample_type val = _val;
        
	_val = _val + _curr_dt * sample;
	if (_val > one)
            _val = one;
	else if (_val < zero)
            _val = zero;
	return value_type { val };
    }

    void update (const range& samples)
    {
        sound::generate_frames (samples, [&] () -> value_type {
                return this->simple_envelope::update ();
            });
    }

    template <class Range2>
    void update (const Range2& samples)
    {
        sound::generate_frames (samples, [&] () {
                return typename Range2::value_type (
                    this->simple_envelope::update ());
            });
    }

    void press ()
    { _curr_dt = _rise_dt; }

    void release ()
    { _curr_dt = _fall_dt; }

    bool finished ()
    { return _val <= 0.0f; }
    
private:
    float       _rise_dt;
    float       _fall_dt;
    float       _curr_dt;
    sample_type _val;
};

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SIMPLE_ENVELOPE_H */
