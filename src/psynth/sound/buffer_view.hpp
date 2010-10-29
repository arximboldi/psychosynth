/**
 *  Time-stamp:  <2010-10-29 13:18:16 raskolnikov>
 *
 *  @file        buffer_view.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 21 12:41:26 2010
 *
 *  Buffer views.
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

#ifndef PSYNTH_BUFFER_VIEW_H_
#define PSYNTH_BUFFER_VIEW_H_

#include <cstddef>
#include <iterator>
#include <psynth/base/compat.hpp>
#include "iterator_from_2d.hpp"

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
   \class buffer_view
   \ingroup BufferViewModel FrameBasedModel

   \brief A lightweight object that interprets memory as a 1D array of
   frames. Models BufferViewConcept, FrameBasedConcept,
   HasDynamicXStepTypeConcept, HasDynamicYStepTypeConcept,
   HasTransposedTypeConcept

   Buffer view consists of a frame iterator and the buffer size.

   Buffer views to buffers are what ranges are to STL containers. They
   are lightweight objects, that don't own the frames. It is the
   user's responsibility that the underlying data remains valid for
   the lifetime of the buffer view.

   Similar to iterators and ranges, constness of views does not extend
   to constness of frames.  A const \p buffer_view does not allow
   changing its location in memory (resizing, moving) but does not
   prevent one from changing the frames. The latter requires an buffer
   view whose value_type is const.

   Buffers have interfaces consistent with STL 1D random access
   containers, so they can be used directly in STL algorithms like:

   \code
   std::fill (buf.begin(), buf.end (), zero_frame);
   \endcode

   In addition, horizontal, vertical and 2D random access iterators
   are provided.

   Note also that \p buffer_view does not require that its element
   type be a frame. It could be instantiated with a locator whose \p
   value_type models only \p Regular. In this case the buffer view
   models the weaker RandomAccess2DBufferViewConcept, and does not
   model FrameBasedConcept.  Many generic algorithms don't require the
   elements to be frames.
*/
template <typename Iterator>
// Models 1D Frame Iterator
class buffer_view
{
public:
    /** @todo Use iterator_traits ?? */
    typedef typename Iterator::value_type                value_type;
    typedef typename Iterator::reference                 reference;
    typedef typename Iterator::difference_type           difference_type;
    typedef buffer_view<typename Iterator::const_type>   const_type;
    typedef Iterator                                     iterator;
    typedef std::reverse_iterator<iterator>              reverse_iterator;
    typedef std::size_t                                  size_type;

    template <typename Deref>
    struct add_deref
    {
        typedef buffer_view <typename Iterator::template add_deref<Deref>::type>
	type;

	static type make (const buffer_view<Iterator>& bv, const Deref& d)
	{
	    return type (
		bv.size (),
		Iterator::template add_deref<Deref>::make (iv.frames (), d));
	}
    };

    buffer_view ()
	: _size (0, 0)
    {}
    
    template <typename View>
    buffer_view (const View& bv)
	: _size (bv.size ())
	, _frames(bv.frames()) {}

    template <typename I2>
    buffer_view (const coord_t& sz, const I2& it)
	: _size (sz)
	, _frames (it)
    {}
    
    template <typename View>
    buffer_view& operator= (const View& bv)
    {
	_frames = bv.frames ();
	_size   = bv.size ();
	return *this;
    }
    
    buffer_view& operator= (const buffer_view& bv)
    {
	_frames = bv.frames();
	_size   = bv.size();
	return *this;
    }

    template <typename View>
    bool operator== (const View& v) const
    {
	return frames () == v.frames() && size () == v.size();
    }
    
    template <typename View>
    bool operator!= (const View& v) const
    {
	return !(*this == v);
    }

    template <typename L2>
    friend void swap (buffer_view<L2>& x, buffer_view<L2>& y);

    const point_t&   size () const
    {
	return _size;
    }

    iterator& frames ()
    {
	return _frames;
    }

    std::size_t frame_size ()
    {
	return memunit_step (begin ());
    }
    
    std::size_t num_samples () const
    {
	return sound::num_samples<value_type>::value;
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
    template <typename L2> friend class buffer_view;

    point_t   _size;
    iterator  _frames;
};

template <typename I2> 
inline void swap (buffer_view<I2>& x, buffer_view<I2>& y)
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
struct sample_type<buffer_view<I> > : public sample_type<I> {}; 

template <typename I>
struct channel_space_type<buffer_view<I> > : public channel_space_type<I> {}; 

template <typename I>
struct sample_mapping_type<buffer_view<I> > : public sample_mapping_type<I> {}; 

template <typename I>
struct is_planar<buffer_view<I> > : public is_planar<I> {}; 

} /* namespace sound */
} /* namespace psynth */

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif /* PSYNTH_BUFFER_VIEW_H_ */
