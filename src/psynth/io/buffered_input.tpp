/**
 *  Time-stamp:  <2011-03-17 15:19:36 raskolnikov>
 *
 *  @file        buffered_input.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Mar 17 15:02:45 2011
 *
 *  @brief Buffered input template implementation.
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

#ifndef PSYNTH_IO_BUFFERED_INPUT_TPP_
#define PSYNTH_IO_BUFFERED_INPUT_TPP_

#include <psynth/io/buffered_input.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class R1, class R2>
struct buffered_input_put_fn
{
    template <class Input>
    std::size_t operator () (Input& in, const R1& data)
    { return in.template take<R1> (data); }
};

template <class R>
struct buffered_input_put_fn<R, R>
{
    template <class Input>
    std::size_t operator () (Input& in, const R& data)
    { return in.input ().take (data); }
};

template <class Ir, class Op>
std::size_t buffered_input_impl<Ir, Op>::take (const range& data)
{
    buffered_input_put_fn <range, input_range> p;
    return p (*this, data);
}

template <class Ir, class Op>
template <class Range>
std::size_t buffered_input_impl<Ir, Op>::take (const Range& data)
{
    std::size_t block_size  = _buffer.size ();
    std::size_t total       = data.size ();
    std::size_t read        = 0;
    std::size_t old_read    = 1; // Do not get into an infinite loop
                                 // when the device refuses to write.

    while (old_read != read && read < total)
    {
        const std::size_t to_read = std::min (block_size, total - read);
        auto src = sub_range (data, read, to_read);
        auto dst = sub_range (sound::range (_buffer), 0, to_read);
        old_read = read;
        read += _input_ptr->take (dst);
        copy_and_convert_frames (dst, src);
    }

    return read;
}

template <class Ir, class Op>
void buffered_async_input_impl<Ir, Op>::fit_buffer ()
{
    this->_input_ptr->check_idle ();
    std::size_t new_size = this->_input_ptr->get_buffer_size ();
    if (new_size != this->_buffer.size ())
        this->set_buffer_size (this->_input_ptr->get_buffer_size ());
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_BUFFERED_INPUT_TPP_ */
