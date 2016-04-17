/**
 *  Time-stamp:  <2011-06-16 22:09:34 raskolnikov>
 *
 *  @file        oscillator.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 21:56:59 2011
 *
 *  @brief Oscillator template implementation.
 *  @todo The bits32sf casts should be handled properly.
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

#ifndef PSYNTH_SYNTH_OSCILLATOR_TPP_
#define PSYNTH_SYNTH_OSCILLATOR_TPP_

#include <psynth/synth/oscillator.hpp>

namespace psynth
{
namespace synth
{

template <typename G>
wave_table<sound::mono32sf_buffer> oscillator<G>::s_wave_table;

template <typename G>
bool oscillator<G>::s_table_init = false;


/**
 * @todo This is not reentrant!
 */
template <class G>
void oscillator<G>::initialize_table ()
{
    s_wave_table.fill (default_table_size, _gen);
    s_table_init = true;
}

template <class G>
template <class Range1>
void oscillator<G>::update (const Range1& out_buf)
{
    typedef typename Range1::value_type frame_type;

    generate_frames (out_buf, [&] () -> frame_type {
            frame_type ret { this->_gen (this->_x) * this->_ampl };
            this->_x += this->_speed;
            return ret;
        });

    _x = base::phase (_x);
}

template <class G>
template <class Range1, class Range2>
void oscillator<G>::update_fm (const Range1& out_buf, const Range2& mod_buf)
{
    typedef typename Range2::value_type modval;
    typedef typename Range1::value_type outval;

    transform_frames (mod_buf, out_buf, [&] (modval m) -> outval {
            auto ret = this->_gen (this->_x) * this->_ampl;
            this->_x += (this->_freq + this->_freq * (sound::bits32sf) (m))
                / this->_frame_rate;
            return outval { ret };
        });
    this->_x = base::phase (_x);
}

template <class G>
template <class Range1, class Range2>
void oscillator<G>::update_pm (const Range1& out_buf, const Range2& mod_buf)
{
    typedef typename Range2::value_type modval;
    typedef typename Range1::value_type outval;

    transform_frames (mod_buf, out_buf, [&] (modval m) -> outval {
            auto ret = this->_gen (this->_x + (sound::bits32sf) m) * this->_ampl;
            this->_x += this->_speed;
            return outval { ret };
        });
    _x = base::phase (_x);
}

template <class G>
template <class Range1, class Range2>
void oscillator<G>::update_am (const Range1& out_buf, const Range2& mod_buf)
{
    typedef typename Range2::value_type modval;
    typedef typename Range1::value_type outval;

    transform_frames (mod_buf, out_buf, [&] (modval m) -> outval {
            auto ret = this->_gen (this->_x) * this->_ampl * (sound::bits32sf) m;
            this->_x += this->_speed;
            return outval { ret };
        });
    _x = base::phase (_x);
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_OSCILLATOR_TPP_ */
