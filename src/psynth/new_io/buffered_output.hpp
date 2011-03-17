/**
 *  Time-stamp:  <2011-03-16 22:46:34 raskolnikov>
 *
 *  @file        buffered_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 18:27:16 2011
 *
 *  Buffered output.
 *
 *  @todo Implement remaining constructors.
 *
 *  @todo Think about the complexity of wrapping async_output
 *  polymorphically. Actually it could be wise to make it pure
 *  abstract and turn buffered_async_output into a real adapter.
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

#ifndef PSYNTH_IO_BUFFERED_OUTPUT_H_
#define PSYNTH_IO_BUFFERED_OUTPUT_H_

#include <type_traits>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/new_io/output.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class Range, class OutputPtr> 
class buffered_output_impl : public output<Range>
{
public:
    typedef Range range;
    
    typedef typename std::remove_pointer<OutputPtr>::type output_type;
    typedef typename output_type::range                   output_range;

    typedef typename sound::buffer_type_from_range<output_range>::type
    buffer_type;

    buffered_output_impl (OutputPtr output_ptr = 0)
        : _buffer (default_output_buffer_size)
        , _output_ptr (output_ptr)
    {}
    
    std::size_t put (const range& data);
    
    template <class Range2>
    std::size_t put (const Range2& data);

    output_type& output ()
    { return *_output_ptr; }

    const output_type& output () const
    { return *_output_ptr; }

    const buffer_type& buffer () const
    { return _buffer; }
    
    void set_buffer_size (std::size_t new_size)
    { _buffer.recreate (new_size); }

protected:
    void set_output (OutputPtr ptr)
    {
        _output_ptr = ptr;
        this->fit_buffer ();
    }

    buffer_type _buffer;
    OutputPtr   _output_ptr;
};


template <class Range, class OutputPtr> 
class buffered_async_output_impl :
        public buffered_output_impl<Range, OutputPtr>
{
public:
    typedef  buffered_output_impl<Range, OutputPtr> base;
    
    buffered_async_output_impl (OutputPtr output_ptr = 0)
        : base (output_ptr)
    {}

    void fit_buffer ();
};

} /* namespace detail */


template <class Range, class OutputPtr>
class buffered_output_adapter :
    public detail::buffered_output_impl<Range, OutputPtr>
{
public:
    typedef detail::buffered_output_impl<Range, OutputPtr> base;
    
    buffered_output_adapter (OutputPtr out = 0)
        : base (out)
    { }
    
    void set_output (OutputPtr out = 0)
    { base::set_output (out); }
};

template <class Range, class OutputPtr>
class buffered_async_output_adapter :
    public detail::buffered_output_impl<Range, OutputPtr>
{
public:
    typedef detail::buffered_output_impl<Range, OutputPtr> base;
    
    buffered_async_output_adapter (OutputPtr out = 0)
        : base (out)
    { this->fit_buffer (); }
    
    void set_output (OutputPtr out = 0)
    {
        this->set_output (out);
        this->fit_buffer ();
    }
};


template <class Range, class Output>
class buffered_async_output :
    public detail::buffered_async_output_impl<Range, Output*>
{
public:   
    typedef detail::buffered_async_output_impl<Range, Output*> base;

    /**
     * @todo Use std::forward when available.
     */
    template <typename... Args>
    buffered_async_output (Args... args)
        : base (&_output)
        , _output (args...)
    {}

private:
    Output _output;
};


template <class Range, class Output>
class buffered_output :
    public detail::buffered_output_impl<Range, Output*>
{   
public:
    typedef detail::buffered_output_impl<Range, Output*> base;
 
    template <typename... Args>
    buffered_output (Args... args)
        : base (&_output)
        , _output (args...)
    {}

private:
    Output _output;
};

namespace detail
{

template <class OutputPtr>
struct is_async_ptr : public std::is_base_of<
    async_base,
    typename std::remove_pointer<OutputPtr>::type> {};

} /* namespace detail */

template <class Range, class OutputPtr>
typename std::enable_if<
    !detail::is_async_ptr<OutputPtr>::value,
    buffered_output_adapter<Range, OutputPtr>
    >::type
make_buffered_output (OutputPtr out)
{
    return buffered_output_adapter<Range, OutputPtr> (out);
}

template <class Range, class OutputPtr>
typename std::enable_if<
    detail::is_async_ptr<OutputPtr>::value,
    buffered_async_output_adapter<Range, OutputPtr>
    >::type
make_buffered_output (OutputPtr out)
{
    return buffered_async_output_adapter<Range, OutputPtr> (out);
}

} /* namespace io */
} /* namespace psynth */

#include <psynth/new_io/buffered_output.tpp>

#endif /* PSYNTH_IO_BUFFERED_OUTPUT_H_ */

