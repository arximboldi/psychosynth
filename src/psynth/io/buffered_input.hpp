/**
 *  Time-stamp:  <2011-06-07 19:55:40 raskolnikov>
 *
 *  @file        buffered_input.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Mar 17 15:02:10 2011
 *
 *  @brief Buffered input.
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

#ifndef PSYNTH_IO_BUFFERED_INPUT_H_
#define PSYNTH_IO_BUFFERED_INPUT_H_

#include <psynth/base/type_traits.hpp>
#include <psynth/io/input.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class Range, class InputPtr>
class buffered_input_impl : public input<Range>
{
public:
    typedef Range range;

    typedef typename base::pointee<InputPtr>::type input_type;
    typedef typename input_type::range                   input_range;

    typedef typename sound::buffer_from_range<input_range>::type
    buffer_type;

    buffered_input_impl (InputPtr input_ptr = 0)
        : _buffer (default_input_buffer_size)
        , _input_ptr (input_ptr)
    {}

    std::size_t take (const range& data);

    template <class Range2>
    std::size_t take (const Range2& data);

    input_type& input ()
    { return *_input_ptr; }

    const input_type& input () const
    { return *_input_ptr; }

    const buffer_type& buffer () const
    { return _buffer; }

    void set_buffer_size (std::size_t new_size)
    { _buffer.recreate (new_size); }

protected:
    void set_input (InputPtr ptr)
    {
        _input_ptr = ptr;
        this->fit_buffer ();
    }

    buffer_type _buffer;
    InputPtr   _input_ptr;
};


template <class Range, class InputPtr>
class buffered_async_input_impl :
        public buffered_input_impl<Range, InputPtr>
{
public:
    typedef  buffered_input_impl<Range, InputPtr> base;

    buffered_async_input_impl (InputPtr input_ptr = 0)
        : base (input_ptr)
    {}

    void fit_buffer ();
};

} /* namespace detail */


template <class Range, class InputPtr>
class buffered_input_adapter :
    public detail::buffered_input_impl<Range, InputPtr>
{
public:
    typedef detail::buffered_input_impl<Range, InputPtr> base;

    buffered_input_adapter (InputPtr in = 0)
        : base (in)
    { }

    void set_input (InputPtr in = 0)
    { base::set_input (in); }
};

template <class Range, class InputPtr>
class buffered_async_input_adapter :
    public detail::buffered_input_impl<Range, InputPtr>
{
public:
    typedef detail::buffered_input_impl<Range, InputPtr> base;

    buffered_async_input_adapter (InputPtr in = 0)
        : base (in)
    { this->fit_buffer (); }

    void set_input (InputPtr in = 0)
    {
        this->set_input (in);
        this->fit_buffer ();
    }
};


template <class Range, class Input>
class buffered_async_input :
    public detail::buffered_async_input_impl<Range, Input*>
{
public:
    typedef detail::buffered_async_input_impl<Range, Input*> base;

    /**
     * @todo Use std::forward when available.
     */
    template <typename... Args>
    buffered_async_input (Args... args)
        : base (&_input)
        , _input (args...)
    {}

private:
    Input _input;
};


template <class Range, class Input>
class buffered_input :
    public detail::buffered_input_impl<Range, Input*>
{
public:
    typedef detail::buffered_input_impl<Range, Input*> base;

    template <typename... Args>
    buffered_input (Args... args)
        : base (&_input)
        , _input (args...)
    {}

private:
    Input _input;
};

namespace detail
{

template <class InputPtr>
struct is_async_ptr : public std::is_base_of<
    async_base,
    typename boost::pointee<InputPtr>::type> {};

} /* namespace detail */

template <class Range, class InputPtr>
typename std::enable_if<
    !detail::is_async_ptr<InputPtr>::value,
    buffered_input_adapter<Range, InputPtr>
    >::type
make_buffered_input (InputPtr in)
{
    return buffered_input_adapter<Range, InputPtr> (in);
}

template <class Range, class InputPtr>
typename std::enable_if<
    detail::is_async_ptr<InputPtr>::value,
    buffered_async_input_adapter<Range, InputPtr>
    >::type
make_buffered_input (InputPtr in)
{
    return buffered_async_input_adapter<Range, InputPtr> (in);
}

} /* namespace io */
} /* namespace psynth */

#include <psynth/io/buffered_input.tpp>

#endif /* PSYNTH_IO_BUFFERED_INPUT_H_ */
