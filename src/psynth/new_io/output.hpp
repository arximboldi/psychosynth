/**
 *  Time-stamp:  <2011-03-09 13:49:37 raskolnikov>
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

#include <psynth/new_io/output.tpp>

namespace psynth
{
namespace io
{

/**
 * Output device base abstract class.
 */
template <typename Range>
class output
{
public:
    typedef Range range;

    /**
     * Write a bunch of data into the device.
     */
    virtual std::size_t put (const range& data) = 0;
};

/**
 * Dummy output class.
 */
template <typename Range>
class dummy_output : public output<Range>
{
public:
    typedef Range range;
    
    virtual std::size_t put (const range& data)
    {
        detail::dummy_output_put_impl ();
        return data.size ();
    }
};

/**
 * An output device with asynchronous operation base class.
 */
template <typename Range>
class async_output : public output<Range>,
                     public virtual async_base
{
public:
    virtual std::size_t buffer_size () const = 0;
};

/**
 * Utility function to implement output devices in term of a raw
 * output device.
 */
template <typename RawOutput, typename Range>
std::size_t put_on_raw (RawOutput& out, const Range& data);

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OUTPUT_H */

