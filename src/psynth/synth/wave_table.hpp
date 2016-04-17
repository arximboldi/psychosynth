/**
 *  Time-stamp:  <2011-03-18 22:56:33 raskolnikov>
 *
 *  @file        wave_table.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  @brief Simple wave_table implementation.
 */

/*
 *  Copyright (C) 2007 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_SYNTH_WAVETABLE_H
#define PSYNTH_SYNTH_WAVETABLE_H

#include <cmath>

#include <psynth/sound/buffer.hpp>
#include <psynth/base/misc.hpp>

namespace psynth
{
namespace synth
{

template <class Buffer>
class wave_table
{
public:
    typedef typename Buffer::range::value_type frame_type;

    wave_table () = default;
    wave_table (const wave_table&) = default;
    wave_table (wave_table&&) = default;
    wave_table& operator= (const wave_table&) = default;

    wave_table (std::size_t size)
        : _table (size) {}

    template <typename Func>
    wave_table (std::size_t size, Func func)
        : _table (size)
    {
	fill (func);
    }

    frame_type get (float x) const;

    template <class Func>
    void fill (std::size_t new_size, Func func)
    {
        _table.recreate (new_size);
	fill(func);
    }

    template <class Func>
    void fill (Func func);

private:
    Buffer _table;
};

} /* namespace synth */
} /* namespace psynth */

#include <psynth/synth/wave_table.tpp>

#endif /* PSYNTH_SYNTH_WAVETABLE_H */
