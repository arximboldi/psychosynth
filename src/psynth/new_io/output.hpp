/**
 *  Time-stamp:  <2011-03-08 23:53:58 raskolnikov>
 *
 *  @file        output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 18:02:42 2011
 *
 *  Output base.
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

#ifndef PSYNTH_IO_OUTPUT_HPP_
#define PSYNTH_IO_OUTPUT_HPP_

#include <psynth/sound/forwards.hpp>
#include <psynth/new_io/async_base.hpp>

namespace psynth
{
namespace io
{

template <typename Range>
class output
{
public:
    typedef Range range;
    virtual std::size_t put (const range& data) = 0;
};


template <typename Range>
class async_output : public output<Range>,
                     public virtual async_base
{
public:
    virtual std::size_t buffer_size () const = 0;
};

template <typename RawOutput, typename Range>
std::size_t put_on_raw (RawOutput& out, const Range& data)
{
    std::size_t block_size = out.buffer_size ();
    std::size_t total      = data.size (); // int is an optimization
    std::size_t written    = 0;
    
    while (written < total)
    {
        auto block = sub_range (
            data, written, std::min (block_size, total - written));

        // TODO: Make sure that this if is optimized away.
        if (sound::is_planar<Range>::value)
        {
            written += out.put_i (block.begin (), block.size ());
        }
        else
        {
            auto it = block.begin ();
            written += out.put_w (&it, block.size ());
        }
    }

    return written;
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OUTPUT_H */

