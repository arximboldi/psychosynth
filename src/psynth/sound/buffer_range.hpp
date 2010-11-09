/**
 *  Time-stamp:  <2010-11-09 14:08:25 raskolnikov>
 *
 *  @file        buffer_range.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 21 12:41:26 2010
 *
 *  Buffer ranges.
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

#ifndef PSYNTH_SOUND_BUFFER_RANGE_H_
#define PSYNTH_SOUND_BUFFER_RANGE_H_

#include <cstddef>
#include <iterator>

#include <psynth/base/compat.hpp>
#include <psynth/sound/frame.hpp>

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4244)
// conversion from 'psynth::image<V,Alloc>::size_type' to 'int', possible
// loss of data (visual studio compiler doesn't realize that the two
// types are the same)
//#endif

namespace psynth
{
namespace sound
{

/**
   \class buffer_range
   \ingroup BufferRangeModel FrameBasedModel

   \brief A lightweight object that interprets memory as a 1D array of
   frames. Models BufferRangeConcept, FrameBasedConcept,
   HasDynamicXStepTypeConcept, HasDynamicYStepTypeConcept,
   HasTransposedTypeConcept

   Buffer range consists of a frame iterator and the buffer size.

   Buffer ranges to buffers are what ranges are to STL containers. They
   are lightweight objects, that don't own the frames. It is the
   user's responsibility that the underlying data remains valid for
   the lifetime of the buffer range.

   Similar to iterators and ranges, constness of ranges does not extend
   to constness of frames.  A const \p buffer_range does not allow
   changing its location in memory (resizing, moving) but does not
   prevent one from changing the frames. The latter requires an buffer
   range whose value_type is const.

   Buffers have interfaces consistent with STL 1D random access
   containers, so they can be used directly in STL algorithms like:

   \code
   std::fill (buf.begin(), buf.end (), zero_frame);
   \endcode

   In addition, horizontal, vertical and 2D random access iterators
   are provided.

   Note also that \p buffer_range does not require that its element
   type be a frame. It could be instantiated with a locator whose \p
   value_type models only \p Regular. In this case the buffer range
   models the weaker RandomAccess2DBufferRangeConcept, and does not
   model FrameBasedConcept.  Many generic algorithms don't require the
   elements to be frames.
*/
template <typename Iterator>
// Models 1D Frame Iterator
class buffer_range
{
public:
    /** @todo Use iterator_traits ?? */
    typedef typename std::iterator_traits<Iterator>::value_type
    value_type;

    typedef typename std::iterator_traits<Iterator>::reference
    reference;

    typedef typename std::iterator_traits<Iterator>::difference_type
    difference_type;
    
    typedef buffer_range<typename const_iterator_type<Iterator>::type>
    const_type;

    typedef Iterator                                     iterator;
    typedef std::reverse_iterator<iterator>              reverse_iterator;
    typedef std::ptrdiff_t                               size_type;

    template <typename Deref>
    struct add_deref
    {
	typedef buffer_range <
	    typename iterator_add_deref<Iterator, Deref>::type> type;
	static type make (const buffer_range<Iterator>& bv, const Deref& d)
	{
	    return type (bv.size (),
			 iterator_add_deref<Iterator, Deref>::make (
			     bv.frames (), d));
	}
    };

    buffer_range ()
	: _size (0)
    {}

    /** We need a non const constructor to copy from buffers. */
    template <typename Range>
    buffer_range (Range& bv)
	: _size (bv.size ())
	, _frames (bv.frames ()) {}

    template <typename Range>
    buffer_range (const Range& bv)
	: _size (bv.size ())
	, _frames (bv.frames ()) {}

    template <typename I2>
    buffer_range (const size_type& sz, const I2& it)
	: _size (sz)
	, _frames (it)
    {}
    
    template <typename Range>
    buffer_range& operator= (const Range& bv)
    {
	_frames = bv.frames ();
	_size   = bv.size ();
	return *this;
    }
    
    buffer_range& operator= (const buffer_range& bv)
    {
	_frames = bv.frames ();
	_size   = bv.size ();
	return *this;
    }

    template <typename Range>
    bool operator== (const Range& v) const
    {
	return frames () == v.frames () && size () == v.size();
    }
    
    template <typename Range>
    bool operator!= (const Range& v) const
    {
	return !(*this == v);
    }

    template <typename L2>
    friend void swap (buffer_range<L2>& x, buffer_range<L2>& y);

    size_type size () const
    {
	return _size;
    }

    const iterator& frames () const
    {
	return _frames;
    }
    
    std::size_t frame_size () const
    {
	return memunit_step (begin ());
    }
    
    std::size_t num_samples () const
    {
	return psynth::sound::num_samples<value_type>::value;
    }

    iterator begin () const
    {
	return _frames;
    }
    
    iterator end () const
    {
	return begin () + (difference_type) size();
    } // potential performance problem!

    reverse_iterator rbegin() const
    {
	return reverse_iterator (end ());
    }
    
    reverse_iterator rend () const
    {
	return reverse_iterator(begin());
    }
    
    reference operator [] (difference_type i) const
    {
	return begin() [i];
    } // potential performance problem!

    iterator at (difference_type i) const
    {
	return begin () + i;
    }
    //\}@

private:
    template <typename L2> friend class buffer_range;

    size_type   _size;
    iterator    _frames;
};

template <typename I2> 
inline void swap (buffer_range<I2>& x, buffer_range<I2>& y)
{ 
    using std::swap;
    swap (x._size, y._size); 
    swap (x._frames, y._frames); // TODO: Extend further
}

/*
 *
 *  FrameBasedConcept
 *
 */

template <typename I>
struct sample_type<buffer_range<I> > : public sample_type<I> {}; 

template <typename I>
struct channel_space_type<buffer_range<I> > : public channel_space_type<I> {}; 

template <typename I>
struct sample_mapping_type<buffer_range<I> > : public sample_mapping_type<I> {}; 

template <typename I>
struct is_planar<buffer_range<I> > : public is_planar<I> {}; 

/*
 *
 *      HasDynamicXStepTypeConcept
 *
 */

template <typename L>
struct dynamic_step_type<buffer_range<L> >
{
    typedef buffer_range<typename dynamic_step_type<L>::type> type;
};


} /* namespace sound */
} /* namespace psynth */

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif /* PSYNTH_BUFFER_RANGE_H_ */
