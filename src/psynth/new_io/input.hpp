/**
 *  Time-stamp:  <2011-03-17 15:45:40 raskolnikov>
 *
 *  @file        input.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 16:20:55 2011
 *
 *  Input base class.
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

#ifndef PSYNTH_IO_INPUT_H_
#define PSYNTH_IO_INPUT_H_

#include <cstddef>

#include <psynth/sound/forwards.hpp>
#include <psynth/new_io/async_base.hpp>
#include <psynth/new_io/input.tpp>

namespace psynth
{
namespace io
{

const std::size_t default_input_buffer_size = 1024;

/**
 *
 * @todo All the IO interface (actually the output class and derivates
 * too) assume that buffers do not model ranges. This is why we can
 * safely assume a <i>const Range&</i> in this interface. 
 */
template <class Range>
class input
{
public:
    typedef Range range;

    virtual std::size_t take (const Range& r) = 0;
};

/**
 * Dummy input class.
 */
template <typename Range>
class dummy_input : public input<Range>
{
public:
    typedef Range range;
    
    virtual std::size_t take (const range& data)
    {
        detail::dummy_input_take_impl ();
        typename range::value_type zero (
            sound::sample_traits<
                typename sound::sample_type<range>::type
                >::zero_value ());
        fill_frames (data, zero);
        return data.size ();
    }
};

/**
 * @todo Not used yet.
 */
template <typename Range>
class async_input : public input<Range>,
                    public virtual async_base
{
public:
    virtual std::size_t buffer_size () const = 0;
};

/**
 * Dummy input class.
 */
template <typename Range>
class dummy_async_input : public input<Range>
{
public:
    typedef Range range;

    dummy_async_input (std::size_t buffer_size)
        : _buffer_size (buffer_size)
    {}
    
    std::size_t buffer_size () const
    { return _buffer_size; }
    
    virtual std::size_t take (const range& data)
    {
        detail::dummy_input_take_impl ();
        typename range::value_type zero (
            sound::sample_traits<
                typename sound::sample_type<range>::type
                >::zero_value ());
        fill_frames (data, zero);
        return data.size ();
    }

    void start ()
    {
        this->check_idle ();
        this->set_state (async_state::running);
        detail::dummy_input_start_impl ();
    }

    void stop ()
    {
        this->check_running ();
        this->set_state (async_state::idle);
        detail::dummy_input_stop_impl ();
    }
    
private:
    std::size_t _buffer_size;
};


} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_INPUT_H_ */

