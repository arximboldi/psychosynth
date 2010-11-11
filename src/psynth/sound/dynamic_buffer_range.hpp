/**
 *  Time-stamp:  <2010-11-11 21:19:25 raskolnikov>
 *
 *  @file        dynamic_buffer_range.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 19:21:33 2010
 *
 *  Support for run-time instantiated buffer range.
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
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

/*
 *  Copyright 2005-2007 Adobe Systems Incorporated
 * 
 *  Use, modification and distribution are subject to the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 */

#ifndef PSYNTH_SOUND_DYNAMIC_BUFFER_RANGE_HPP
#define PSYNTH_SOUND_DYNAMIC_BUFFER_RANGE_HPP

#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/variant.hpp>
#include <psynth/sound/buffer.hpp>

namespace psynth
{
namespace sound
{

namespace detail
{

template <typename Range>
struct get_const_type
{
    typedef typename Range::const_type type;
};

template <typename Ranges>
struct ranges_get_const_type :
	public boost::mpl::transform<Ranges, get_const_type<boost::mpl::_1> > {};

} /* namespace detail */

template <typename Range>
struct dynamic_step_type;

namespace detail
{

struct dynamic_type_get_num_samples
{   // works for both buffer_range and buffer
    typedef int result_type;
    template <typename T>
    result_type operator () (const T& v) const
    { return num_samples<T>::value; }
};

struct dynamic_type_get_size
{   // works for both buffer_range and buffer
    typedef std::ptrdiff_t result_type;
    template <typename T>
    result_type operator () (const T& v) const
    { return v.size (); }
};

} /* namespace detail */

/**
 * @class dynamic_buffer_range
 *
 * \ingroup BufferRangeModel
 *
 * \brief Represents a run-time specified buffer range. Models
 * HasDynamicXStepTypeConcept, HasDynamicYStepTypeConcept, Note that
 * this class does NOT model BufferRangeConcept
 *
 * Represents a range whose type (color space, layout,
 * planar/interleaved organization, etc) can be specified at run time.
 * It is the runtime equivalent of \p buffer_range.
 *
 * Some of the requirements of BufferRangeConcept, such as the \p
 * value_type typedef cannot be fulfilled, since the language does not
 * allow runtime type specification.  Other requirements, such as
 * access to the frames, would be inefficient to provide. Thus \p
 * dynamic_buffer_range does not fully model BufferRangeConcept.
 * However, many algorithms provide overloads taking runtime specified
 * ranges and thus in many cases \p dynamic_buffer_range can be used in
 * places taking a range.
 *
 * To perform an algorithm on dynamic_buffer_range, put the algorithm
 * in a function object and invoke it by calling \p
 * apply_operation(runtime_range, algorithm_fn);
 */

template <typename BufferRangeTypes>
class dynamic_buffer_range : public variant<BufferRangeTypes>
{
    typedef variant<BufferRangeTypes> parent_type;

public:
    typedef dynamic_buffer_range<
    typename detail::ranges_get_const_type<BufferRangeTypes>::type>
    const_range;
    
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    dynamic_buffer_range ()
	: parent_type () {}
    
    template <typename T>
    explicit dynamic_buffer_range (const T& obj) throw ()
	: parent_type (obj) {}
    
    dynamic_buffer_range (const dynamic_buffer_range& v) 
	: parent_type ((const parent_type&) v)
    {}

    template <typename T>
    dynamic_buffer_range& operator= (const T& obj)
    {
	parent_type::operator= (obj);
	return *this;
    }
    
    dynamic_buffer_range& operator= (const dynamic_buffer_range& v)
    {
	parent_type::operator= ((const parent_type&) v);
	return *this;
    }

    std::size_t num_samples () const
    {
	return apply_operation (*this, detail::dynamic_type_get_num_samples ());
    }
    
    size_type size () const
    {
	return apply_operation (*this, detail::dynamic_type_get_size ());
    }
};

/*
 *
 *     HasDynamicStepTypeConcept
 *
 */
template <typename IVTypes>
struct dynamic_step_type<dynamic_buffer_range<IVTypes> >
{
    typedef dynamic_buffer_range<
	typename boost::mpl::transform<
	    IVTypes, dynamic_step_type<boost::mpl::_1> >::type> type;
};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER_RANGE */
