/**
 *  Time-stamp:  <2011-03-24 18:39:23 raskolnikov>
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
#include <psynth/sound/dynamic_buffer_range.hpp>
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

namespace mpl = boost::mpl;

namespace sound
{

namespace detail
{

template <typename T> struct get_range_type
{
    typedef typename T::range type;
};

template <typename Buffers>
struct buffers_get_ranges_type :
	public mpl::transform<Buffers,
                              mpl::quote1<get_range_type> > {};

template <typename T>
struct get_const_range_type
{
    typedef typename T::const_range type;
};

template <typename Buffers>
struct buffers_get_const_ranges_type :
	public mpl::transform<Buffers,
                              mpl::quote1<get_const_range_type> > {};

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

template <typename AnyRange>  // Models AnyRangeConcept
struct dynamic_buffer_get_range
{
    typedef AnyRange result_type;

    template <typename Buffer>
    result_type operator() (Buffer& img) const
    {
	return result_type (range (img));
    }
};

template <typename AnyConstRange>  // Models AnyConstRangeConcept
struct dynamic_buffer_get_const_range
{
    typedef AnyConstRange result_type;
        
    template <typename Buffer>
    result_type operator () (const Buffer& img) const
    {
	return result_type (const_range (img));
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
 *  does not fully model BufferConcept.  In particular, its \p range
 *  and \p const_range methods return \p dynamic_buffer_range, which
 *  does not fully model BufferRangeConcept. See \p dynamic_buffer_range
 *  for more.
 */
template <typename BufferTypes>
class dynamic_buffer : public variant<BufferTypes>
{
    typedef variant<BufferTypes> parent_type;

public:
    typedef dynamic_buffer_range<
    typename detail::buffers_get_const_ranges_type<BufferTypes>::type>
    const_range;

    typedef dynamic_buffer_range<
	typename detail::buffers_get_ranges_type<BufferTypes>::type>
    range;

    typedef typename range::size_type       size_type;
    typedef typename range::difference_type difference_type;

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
	return apply_operation (*this, detail::dynamic_type_get_size ());
    }
};

/**
 * @{
 * \name range, const_range
 * \brief Get an buffer range from a run-time instantiated buffer
 *
 * \ingroup BufferModel
 *
 * \brief Returns the non-constant-pixel range of any buffer. The
 * returned range is any range.
 */
template <typename Types>  PSYNTH_FORCEINLINE // Models BufferVectorConcept
typename dynamic_buffer<Types>::range
range (dynamic_buffer<Types>& buf)
{ 
    return apply_operation (
	buf, detail::dynamic_buffer_get_range<
	    typename dynamic_buffer<Types>::range> ());
}

/**
 * \brief Returns the constant-pixel range of any buffer. The returned
 * range is any range.
 */
template <typename Types> PSYNTH_FORCEINLINE // Models BufferVectorConcept
typename dynamic_buffer<Types>::const_range
const_range (const dynamic_buffer<Types>& buf)
{ 
    return apply_operation (
	buf, detail::dynamic_buffer_get_const_range <
	    typename dynamic_buffer<Types>::const_range> ());
}
/** @} */

} /* namespace sound */
} /* namespace psynth */

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER */
