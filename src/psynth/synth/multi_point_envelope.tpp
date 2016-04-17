/**
 *  Time-stamp:  <2011-03-21 17:15:08 raskolnikov>
 *
 *  @file        multi_point_envelope.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Mar 19 00:26:18 2011
 *
 *  @brief Multi point envelope template implementation.
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

#ifndef PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_TPP_
#define PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_TPP_

#include <psynth/synth/multi_point_envelope.hpp>

namespace psynth
{
namespace synth
{

template <class S>
void envelope_values<S>::set_asr (point a, point s, point r)
{
    _points  = { {0, 0}, a, s, r };
    _sustain = 2;
}

template <class S>
void envelope_values<S>::set_adsr (point a, point d, point s, point r)
{
    _points  = { {0, 0}, a, d, s, r };
    _sustain = 3;
}

/**
 * @todo Implement sustain!
 */
template <class R, class Vp>
typename multi_point_envelope<R, Vp>::value_type
multi_point_envelope<R, Vp>::update (std::size_t sample)
{
    value_type val (sound::sample_type<R>::type::zero_value ());

    _time += sample * _val->_factor;

    while (_cur_point < _val->size () - 1 &&
	   _val->get (_cur_point+1).dt < _time)
	++_cur_point;

    if (_cur_point < _val->size() - 1)
    {
        val =
            _val->get (_cur_point).val +
            (_val->get (_cur_point + 1).val - _val->get (_cur_point).val) /
            (_val->get (_cur_point + 1).dt  - _val->get (_cur_point).dt) *
            (_time - _val->get (_cur_point).dt);
    }
    else if (_cur_point == _val->size() - 1)
    {
	val   = _val->get (_val->size()-1).val;
	_time = 0.0f;
    }

    return val;
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_TPP_ */
