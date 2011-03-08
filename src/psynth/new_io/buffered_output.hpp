/**
 *  Time-stamp:  <2011-03-08 19:07:54 raskolnikov>
 *
 *  @file        buffered_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 18:27:16 2011
 *
 *  Buffered output.
 *
 *  @todo Implement remaining constructors.
 *  @todo Make compatible with file output! 
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
    typedef Range                        range;
    typedef std::decay<OutputPtr>::type  output;
    typedef typename output::range       output_range;

    buffered_output_impl (OutputPtr output_ptr = 0)
        : _buffer ()
        , _output_ptr (output_ptr)
    {}
    
    std::size_t buffer_size () const
    { return _output_ptr->buffer_size (); }
    
    std::size_t put (const range& data);
    
    template <class Range>
    std::size_t put (const Range& data);

    void fit_buffer ();

    OutputPtr output ()
    { return _output_ptr; }

    const OutputPtr output () const
    { return _output_ptr; }
    
protected:
    void set_output (OutputPtr ptr)
    {
        _output_ptr = ptr;
        fit_buffer ();
    }

    typedef sound::buffer_type_from_range<output_range>::type buffer_type;

    buffer_type _buffer;
    OutputPtr   _output_ptr;
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
    {
        fit_buffer ();
    }
    
    void set_output (OutputPtr out = 0)
    { base::set_output (out); }
};

template <class Range, class Output>
class buffered_output :
    public detail::buffered_output_impl<Range, Output*>
{
    typedef detail::buffered_output_impl<Range, Output*> base;
    
    template <typename... Args>
    buffered_output (Args... args)
        : base (&_output)
        , _output (args)
    {}
    
public:
    Output _output;
};

} /* namespace io */
} /* namespace psynth */

#incude <psynth/new_io/buffered_output.tpp>

#endif /* PSYNTH_IO_BUFFERED_OUTPUT_H_ */

