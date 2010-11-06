/**
 *  Time-stamp:  <2010-11-04 18:04:27 raskolnikov>
 *
 *  @file        dynamic_buffer_view.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 19:21:33 2010
 *
 *  Support for run-time instantiated buffer view.
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

#ifndef PSYNTH_SOUND_DYNAMIC_BUFFER_VIEW_HPP
#define PSYNTH_SOUND_DYNAMIC_BUFFER_VIEW_HPP

#include <psynth/sound/buffer_view.hpp>
#include <psynth/sound/variant.hpp>
#include <psynth/sound/buffer.hpp>

namespace psynth
{
namespace sound
{

namespace detail
{

template <typename View>
struct get_const_type
{
    typedef typename View::const_type type;
};

template <typename Views>
struct views_get_const_type :
	public boost::mpl::transform<Views, get_const_type<boost::mpl::_1> > {};

} /* namespace detail */

template <typename View>
struct dynamic_step_type;

namespace detail
{

struct dynamic_type_get_num_samples
{   // works for both buffer_view and buffer
    typedef int result_type;
    template <typename T>
    result_type operator () (const T& v) const
    { return num_samples<T>::value; }
};

struct dynamic_type_get_dimensions
{   // works for both buffer_view and buffer
    typedef std::ptrdiff_t result_type;
    template <typename T>
    result_type operator () (const T& v) const
    { return v.size (); }
};

} /* namespace detail */

/**
 * @class dynamic_buffer_view
 *
 * \ingroup BufferViewModel
 *
 * \brief Represents a run-time specified buffer view. Models
 * HasDynamicXStepTypeConcept, HasDynamicYStepTypeConcept, Note that
 * this class does NOT model BufferViewConcept
 *
 * Represents a view whose type (color space, layout,
 * planar/interleaved organization, etc) can be specified at run time.
 * It is the runtime equivalent of \p buffer_view.
 *
 * Some of the requirements of BufferViewConcept, such as the \p
 * value_type typedef cannot be fulfilled, since the language does not
 * allow runtime type specification.  Other requirements, such as
 * access to the frames, would be inefficient to provide. Thus \p
 * dynamic_buffer_view does not fully model BufferViewConcept.
 * However, many algorithms provide overloads taking runtime specified
 * views and thus in many cases \p dynamic_buffer_view can be used in
 * places taking a view.
 *
 * To perform an algorithm on dynamic_buffer_view, put the algorithm
 * in a function object and invoke it by calling \p
 * apply_operation(runtime_view, algorithm_fn);
 */

template <typename BufferViewTypes>
class dynamic_buffer_view : public variant<BufferViewTypes>
{
    typedef variant<BufferViewTypes> parent_type;

public:
    typedef dynamic_buffer_view<
    typename detail::views_get_const_type<BufferViewTypes>::type>
    const_view;
    
    typedef std::ptrdiff_t size_type;

    dynamic_buffer_view ()
	: parent_type () {}
    
    template <typename T>
    explicit dynamic_buffer_view (const T& obj)
	: parent_type (obj) {}
    
    dynamic_buffer_view (const dynamic_buffer_view& v)
	: parent_type ((const parent_type&) v)
    {}

    template <typename T>
    dynamic_buffer_view& operator= (const T& obj)
    {
	parent_type::operator= (obj);
	return *this;
    }
    
    dynamic_buffer_view& operator= (const dynamic_buffer_view& v)
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
	return apply_operation (*this, detail::dynamic_type_get_dimensions());
    }
};

/*
 *
 *     HasDynamicStepTypeConcept
 *
 */
template <typename IVTypes>
struct dynamic_step_type<dynamic_buffer_view<IVTypes> >
{
    typedef dynamic_buffer_view<
	typename boost::mpl::transform<
	    IVTypes, dynamic_step_type<boost::mpl::_1> >::type> type;
};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER_VIEW */
