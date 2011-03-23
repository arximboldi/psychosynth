/**
 *  Time-stamp:  <2011-03-21 21:03:27 raskolnikov>
 *
 *  @file        buffered_output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 19:24:02 2011
 *
 *  Buffered output implementation.
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

#ifndef PSYNTH_IO_BUFFERED_OUTPUT_TPP_
#define PSYNTH_IO_BUFFERED_OUTPUT_TPP_

#include <psynth/sound/buffer.hpp>
#include <psynth/io/buffered_output.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class R1, class R2>
struct buffered_output_put_fn
{
    template <class Output>
    std::size_t operator () (Output& out, const R1& data)
    { return out.template put<R1> (data); }
};

template <class R>
struct buffered_output_put_fn<R, R>
{
    template <class Output>
    std::size_t operator () (Output& out, const R& data)
    { return out.output ().put (data); }
};

template <class Ir, class Op>
std::size_t buffered_output_impl<Ir, Op>::put (const range& data)
{
    buffered_output_put_fn <range, output_range> p;
    return p (*this, data);
}

template <class Ir, class Op>
template <class Range>
std::size_t buffered_output_impl<Ir, Op>::put (const Range& data)
{
    std::size_t block_size  = _buffer.size ();
    std::size_t total       = data.size ();
    std::size_t written     = 0;
    std::size_t old_written = 1; // Do not get into an infinite loop
                                 // when the device refuses to write.
    
    while (old_written != written && written < total)
    {
        const std::size_t to_write = std::min (block_size, total - written); 
        auto src = sub_range (data, written, to_write);
        auto dst = sub_range (sound::range (_buffer), 0, to_write);
        old_written = written;
        copy_and_convert_frames (src, dst);
        written += _output_ptr->put (dst);
    }
    
    return written;
}

template <class Ir, class Op>
void buffered_async_output_impl<Ir, Op>::fit_buffer ()    
{
    this->_output_ptr->check_idle ();
    std::size_t new_size = this->_output_ptr->buffer_size ();
    if (new_size != (size_t) this->_buffer.size ())
        this->set_buffer_size (this->_output_ptr->buffer_size ());
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_BUFFERED_OUTPUT_TPP_ */

