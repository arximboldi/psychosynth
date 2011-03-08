/**
 *  Time-stamp:  <2011-03-08 18:50:51 raskolnikov>
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
    { out.template put<R1> (data); }
};

template <class R, class R>
struct buffered_output_put_fn
{
    template <class Output>
    std::size_t operator () (Output& out, const R1& data)
    { out.wrapped ().put (data); }
};

template <class Ir, class Op>
std::size_t buffered_output_adapter_impl<Ir, Op>::put (const range& data)
{
    buffered_output_put_fn <range, output_range> p;
    return p (*this, data);
}

template <class Range>
template <class Ir, class Op>
std::size_t buffered_output_adapter_impl<Ir, Op>::put (const Range& data)
{
    // TODO TODO TODO
}

template <class IR, class OP>
void buffered_output_adapter_impl<Ir, Op>::fit_buffer ()    
{
    std::size_t new_size = _output_ptr->buffer_size ();
    if (new_size != _buffer.size ())
        _buffer.recreate ();
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_BUFFERED_OUTPUT_TPP_ */

