/**
 *  Time-stamp:  <2010-11-05 13:51:45 raskolnikov>
 *
 *  @file        dynamic_buffer.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 18:40:09 2010
 *
 *  A dynamic image.
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

#ifndef PSYNTH_SOUND_DYNAMIC_BUFFER_HPP
#define PSYNTH_SOUND_DYNAMIC_BUFFER_HPP

#include <psynth/sound/buffer.hpp>
#include <psynth/sound/dynamic_buffer_view.hpp>
#include <psynth/sound/apply_operation.hpp>

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4244)
// conversion from 'std::ptrdiff_t' to 'int', possible loss of
// data. even if we static-assert the two types are the same (on
// visual studio 8)
//#endif

namespace psynth
{
namespace sound
{

namespace detail
{

template <typename T> struct get_view_type
{
    typedef typename T::view type;
};

template <typename Buffers>
struct buffers_get_views_type :
	public boost::mpl::transform<Buffers,
				     get_view_type<boost::mpl::_1> > {};

template <typename T>
struct get_const_view_type
{
    typedef typename T::const_view type;
};

template <typename Buffers>
struct buffers_get_const_views_type :
	public boost::mpl::transform<Buffers,
				     get_const_view_type<boost::mpl::_1> > {};

struct recreate_buffer_fnobj
{
    typedef void result_type;
    std::size_t _size;
    unsigned    _alignment;

    recreate_buffer_fnobj (std::size_t size, unsigned alignment)
	: _size (size)
	, _alignment (alignment)
    {}

    template <typename Buffer>
    result_type operator () (Buffer& img) const
    {
	img.recreate (_size, _alignment);
    }
};

template <typename AnyView>  // Models AnyViewConcept
struct dynamic_buffer_get_view
{
    typedef AnyView result_type;

    template <typename Buffer>
    result_type operator() (Buffer& img) const
    {
	return result_type (view (img));
    }
};

template <typename AnyConstView>  // Models AnyConstViewConcept
struct dynamic_buffer_get_const_view
{
    typedef AnyConstView result_type;
        
    template <typename Buffer>
    result_type operator () (const Buffer& img) const
    {
	return result_type (const_view (img));
    }
};

} /* namespace detail */

/**
 * \ingroup BufferModel
 *
 *  \brief Represents a run-time specified buffer. Note it does NOT
 *  model BufferConcept
 *
 *  Represents an buffer whose type (color space, layout,
 *  planar/interleaved organization, etc) can be specified at run
 *  time.  It is the runtime equivalent of \p buffer.  Some of the
 *  requirements of BufferConcept, such as the \p value_type typedef
 *  cannot be fulfilled, since the language does not allow runtime
 *  type specification.  Other requirements, such as access to the
 *  pixels, would be inefficient to provide. Thus \p dynamic_buffer
 *  does not fully model BufferConcept.  In particular, its \p view
 *  and \p const_view methods return \p dynamic_buffer_view, which
 *  does not fully model BufferViewConcept. See \p dynamic_buffer_view
 *  for more.
 */
template <typename BufferTypes>
class dynamic_buffer : public variant<BufferTypes>
{
    typedef variant<BufferTypes> parent_type;

public:
    typedef dynamic_buffer_view<
    typename detail::buffers_get_const_views_type<BufferTypes>::type>
    const_view;

    typedef dynamic_buffer_view<
	typename detail::buffers_get_views_type<BufferTypes>::type>
    view;

    typedef std::ptrdiff_t size_type;

    dynamic_buffer ()
	: parent_type () {}
    
    template <typename T>
    explicit dynamic_buffer (const T& obj)
	: parent_type (obj) {}
    
    template <typename T>
    explicit dynamic_buffer (T& obj, bool do_swap)
	: parent_type (obj, do_swap) {}

    dynamic_buffer (const dynamic_buffer& v)
	: parent_type ((const parent_type&) v)    {}

    template <typename T>
    dynamic_buffer& operator= (const T& obj)
    {
	parent_type::operator= (obj);
	return *this;
    }
    
    dynamic_buffer& operator= (const dynamic_buffer& v)
    {
	parent_type::operator= ((const parent_type&) v);
	return *this;
    }

    void recreate (size_type size, unsigned align = 1)
    {
	apply_operation (*this, detail::recreate_buffer_fnobj (size, align));
    }

    std::size_t num_samples () const
    {
	return apply_operation (*this, detail::dynamic_type_get_num_samples ());
    }

    size_type size () const
    {
	return apply_operation (*this, detail::dynamic_type_get_dimensions ());
    }
};

/**
 * @{
 * \name view, const_view
 * \brief Get an buffer view from a run-time instantiated buffer
 *
 * \ingroup BufferModel
 *
 * \brief Returns the non-constant-pixel view of any buffer. The
 * returned view is any view.
 */
template <typename Types>  PSYNTH_FORCEINLINE // Models BufferVectorConcept
typename dynamic_buffer<Types>::view
view (dynamic_buffer<Types>& buf)
{ 
    return apply_operation (
	buf, detail::dynamic_buffer_get_view<
	    typename dynamic_buffer<Types>::view> ());
}

/**
 * \brief Returns the constant-pixel view of any buffer. The returned
 * view is any view.
 */
template <typename Types> PSYNTH_FORCEINLINE // Models BufferVectorConcept
typename dynamic_buffer<Types>::const_view
const_view (const dynamic_buffer<Types>& buf)
{ 
    return apply_operation (
	buf, detail::dynamic_buffer_get_const_view <
	    typename dynamic_buffer<Types>::const_view> ());
}
/** @} */

} /* namespace sound */
} /* namespace psynth */

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER */
