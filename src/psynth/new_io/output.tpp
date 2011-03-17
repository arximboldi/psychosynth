/**
 *  Time-stamp:  <2011-03-16 20:37:08 raskolnikov>
 *
 *  @file        output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 13:15:52 2011
 *
 *  Template implementation in the output module.
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

#ifndef PSYNTH_IO_OUTPUT_TPP_
#define PSYNTH_IO_OUTPUT_TPP_

#include <iostream> // REMOVE

#include <psynth/base/compat.hpp>
#include <psynth/new_io/output.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <bool IsPlanar>
struct put_on_raw_fn {};

template <>
struct put_on_raw_fn<false>
{
    template <class RawOutput, class Range> PSYNTH_FORCEINLINE
    std::size_t operator () (RawOutput& out, const Range& data)
    {
        return out.put_i (data.frames (), data.size ());
    }
};

template <>
struct put_on_raw_fn<true>
{
    template <class RawOutput, class Range> PSYNTH_FORCEINLINE
    std::size_t operator () (RawOutput& out, const Range& data)
    {
        return out.put_n (
            reinterpret_cast<const void* const*> (&data.frames ()),
            data.size ());
    }
};

} /* namespace detail */

template <typename RawOutput, typename Range>
std::size_t put_on_raw (RawOutput& out, const Range& data)
{
    std::size_t block_size  = out.buffer_size ();
    std::size_t total       = data.size ();
    std::size_t written     = 0;
    std::size_t old_written = 1; // Do not get into an infinite loop
                                 // when the device refuses to write.
    
    while (old_written != written && written < total)
    {
        auto block = sub_range (
            data, written, std::min (block_size, total - written));
        old_written = written;
        written += detail::put_on_raw_fn<sound::is_planar<Range>::value> () (
            out, block);
    }
    
    return written;
}


namespace detail
{

void dummy_output_put_impl ();
void dummy_output_start_impl ();
void dummy_output_stop_impl ();

} /* detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OUTPUT_TPP_ */
