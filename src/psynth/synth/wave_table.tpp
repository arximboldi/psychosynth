/**
 *  Time-stamp:  <2011-03-21 17:15:49 raskolnikov>
 *
 *  @file        wave_table.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 20:11:54 2011
 *
 *  @brief Template implementation of wave_table.
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

#ifndef PSYNTH_SYNTH_WAVE_TABLE_TPP_
#define PSYNTH_SYNTH_WAVE_TABLE_TPP_

#include <psynth/synth/wave_table.hpp>

namespace psynth
{
namespace synth
{

template <class B>
typename wave_table<B>::frame_type wave_table<B>::get (float x) const
{
    auto rng = range (_table);

    float  findex = (rng.size () - 1) * base::phase(x);
    size_t index  = findex;
    float  alpha  = findex - index;

    frame_type out = rng [index];
    out += alpha * (rng [index + 1 >= rng.size () ? 0 : index + 1] - out);

    return out;
}

template <class B>
template <class Func>
void wave_table<B>::fill (Func func)
{
    auto rng = range (_table);
    std::size_t i = 0;
    std::size_t s = rng.size ();
    sound::generate_frames (rng, [&] () -> frame_type {
            return frame_type { func (static_cast<float> (i++) / s) };
        });
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_WAVE_TABLE_TPP_ */
