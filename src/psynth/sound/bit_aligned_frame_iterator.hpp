/**
 *  Time-stamp:  <2010-11-02 11:07:37 raskolnikov>
 *
 *  @file        bit_aligned_frame_iterator.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 18:36:41 2010
 *
 *  Bit aligned frame iterator.
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

#ifndef PSYNTH_SOUND_BIT_ALIGNED_FRAME_ITERATOR_HPP
#define PSYNTH_SOUND_BIT_ALIGNED_FRAME_ITERATOR_HPP

#include <functional>
#include <boost/iterator/iterator_facade.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/base/bit_aligned_frame_reference.hpp>
#include <psynth/base/frame_iterator.hpp>

namespace psynth
{
namespace sound
{

/**
   \defgroup FrameIteratorNonAlignedFrameIterator
   bit_aligned_frame_iterator
   \ingroup FrameIteratorModel

   \brief An iterator over non-byte-aligned frames. Models
   FrameIteratorConcept, FrameBasedConcept,
   MemoryBasedIteratorConcept, HasDynamicXStepTypeConcept
*/

/**
   \brief An iterator over non-byte-aligned frames. Models
   FrameIteratorConcept, FrameBasedConcept,
   MemoryBasedIteratorConcept, HasDynamicXStepTypeConcept
   
   An iterator over frames that correspond to non-byte-aligned bit
   ranges. Examples of such frames are single bit grayscale frame, or
   a 6-bit RGB 222 frame.
   
   \ingroup FrameIteratorNonAlignedFrameIterator FrameBasedModel
*/
template <typename NonAlignedFrameReference>
struct bit_aligned_frame_iterator : public iterator_facade<
    bit_aligned_frame_iterator<
	NonAlignedFrameReference>,
    typename NonAlignedFrameReference::value_type,
    std::random_access_iterator_tag,
    const NonAlignedFrameReference,
    typename NonAlignedFrameReference::bit_range_type::difference_type>
{
private:
    typedef
    iterator_facade<
    bit_aligned_frame_iterator<NonAlignedFrameReference>,
    typename NonAlignedFrameReference::value_type,
    std::random_access_iterator_tag,
    const NonAlignedFrameReference,
    typename NonAlignedFrameReference::bit_range_type::difference_type>
    parent_type;

    template <typename Ref> friend struct bit_aligned_frame_iterator;

    typedef typename NonAlignedFrameReference::bit_range_type bit_range_type;

public:
    typedef typename parent_type::difference_type   difference_type;
    typedef typename parent_type::reference         reference;

    bit_aligned_frame_iterator () {}
    
    bit_aligned_frame_iterator (const bit_aligned_frame_iterator& p)
	: _bit_range (p._bit_range) {}

    bit_aligned_frame_iterator& operator= (const bit_aligned_frame_iterator& p)
    {
	_bit_range = p._bit_range;
	return *this;
    }

    template <typename Ref>
    bit_aligned_frame_iterator (const bit_aligned_frame_iterator<Ref>& p)
	: _bit_range (p._bit_range) {}

    bit_aligned_frame_iterator (reference* ref)
	: _bit_range (ref->bit_range ())
    {}
    
    explicit bit_aligned_frame_iterator (
	typename bit_range_type::byte_type* data, int bit_offset = 0)
	: _bit_range(data,bit_offset)
    {}

    /**
       For some reason operator[] provided by iterator_adaptor returns
       a custom class that is convertible to reference
       
       We require our own reference because it is registered in
       iterator_traits
    */
    reference operator [] (difference_type d) const
    {
	bit_aligned_frame_iterator it = *this;
	it.advance (d);
	return *it;
    }

    reference operator-> () const { return **this; }
    
    const bit_range_type& bit_range () const { return _bit_range; }
          bit_range_type& bit_range ()       { return _bit_range; }

private:
    bit_range_type _bit_range;
    BOOST_STATIC_CONSTANT(int, bit_size = NonAlignedFrameReference::bit_size);

    friend class boost::iterator_core_access;
    reference dereference () const
    { return NonAlignedFrameReference(_bit_range); }
    
    void increment ()
    { ++_bit_range; }

    void decrement ()
    { --_bit_range; }

    void advance (difference_type d)
    { _bit_range.bit_advance (d * bit_size); }

    difference_type distance_to (const bit_aligned_frame_iterator& it) const
    { return _bit_range.bit_distance_to(it._bit_range) / bit_size; }
    
    bool equal (const bit_aligned_frame_iterator& it) const
    { return _bit_range == it._bit_range; }
};

template <typename NonAlignedFrameReference> 
struct const_iterator_type<bit_aligned_frame_iterator<NonAlignedFrameReference> >
{ 
    typedef bit_aligned_frame_iterator<
	typename NonAlignedFrameReference::const_reference> type; 
};

template <typename NonAlignedFrameReference> 
struct iterator_is_mutable<
    bit_aligned_frame_iterator<NonAlignedFrameReference> > :
	public mpl::bool_<NonAlignedFrameReference::is_mutable> {};

template <typename NonAlignedFrameReference> 
struct is_iterator_adaptor<
    bit_aligned_frame_iterator<NonAlignedFrameReference> > :
	public mpl::false_ {};

/*
 *
 *    FrameBasedConcept
 *
 */

template <typename NonAlignedFrameReference>
struct channel_space_type<
    bit_aligned_frame_iterator<NonAlignedFrameReference> > :
	public channel_space_type<NonAlignedFrameReference> {};

template <typename NonAlignedFrameReference>
struct sample_mapping_type<
    bit_aligned_frame_iterator<NonAlignedFrameReference> > :
	public sample_mapping_type<NonAlignedFrameReference> {};

template <typename NonAlignedFrameReference>
struct is_planar<bit_aligned_frame_iterator<NonAlignedFrameReference> > :
    public is_planar<NonAlignedFrameReference> {}; // == false


/*
 *
 *    MemoryBasedIteratorConcept
 *
 */

template <typename NonAlignedFrameReference>
struct byte_to_memunit<bit_aligned_frame_iterator<NonAlignedFrameReference> > :
    public mpl::int_<8> {};

template <typename NonAlignedFrameReference>
inline std::ptrdiff_t memunit_step (
    const bit_aligned_frame_iterator<NonAlignedFrameReference>&)
{ 
    return NonAlignedFrameReference::bit_size; 
}

template <typename NonAlignedFrameReference>
inline std::ptrdiff_t memunit_distance (
    const bit_aligned_frame_iterator<NonAlignedFrameReference>& p1,
    const bit_aligned_frame_iterator<NonAlignedFrameReference>& p2)
{ 
    return
	(p2.bit_range().current_byte() -
	 p1.bit_range().current_byte()) * 8 +
	p2.bit_range().bit_offset() -
	p1.bit_range().bit_offset(); 
}

template <typename NonAlignedFrameReference>
inline void memunit_advance (
    bit_aligned_frame_iterator<NonAlignedFrameReference>& p,
    std::ptrdiff_t diff)
{ 
    p.bit_range().bit_advance(diff);
}

template <typename NonAlignedFrameReference>
inline bit_aligned_frame_iterator<NonAlignedFrameReference>
memunit_advanced (const bit_aligned_frame_iterator<NonAlignedFrameReference>& p,
		  std::ptrdiff_t diff)
{
    bit_aligned_frame_iterator<NonAlignedFrameReference> ret = p;
    memunit_advance (ret, diff);
    return ret;
}

template <typename NonAlignedFrameReference> inline
NonAlignedFrameReference memunit_advanced_ref(
    bit_aligned_frame_iterator<NonAlignedFrameReference> it,
    std::ptrdiff_t diff)
{
    return * memunit_advanced (it, diff);
}


/*
 *
 *    HasDynamicStepTypeConcept
 *
 */

template <typename NonAlignedFrameReference>
struct dynamic_step_type<bit_aligned_frame_iterator<NonAlignedFrameReference> >
{
    typedef memory_based_step_iterator<
	bit_aligned_frame_iterator<NonAlignedFrameReference> > type;
};


/*
 *
 *  iterator_type_from_frame
 *
 */

template <typename B, typename C, typename L, bool M>
struct iterator_type_from_frame<
    const bit_aligned_frame_reference<B,C,L,M>, false, false, false>
{
    typedef bit_aligned_frame_iterator<
	bit_aligned_frame_reference<B,C,L,false> > type;
};

template <typename B, typename C, typename L, bool M>
struct iterator_type_from_frame<
    const bit_aligned_frame_reference<B,C,L,M>, false, false, true>
{
    typedef bit_aligned_frame_iterator<
	bit_aligned_frame_reference<B,C,L,true> > type;
};

template <typename B, typename C, typename L,
	  bool M, bool IsPlanar, bool IsStep, bool IsMutable>
struct iterator_type_from_frame<
    bit_aligned_frame_reference<B,C,L,M>, IsPlanar, IsStep, IsMutable>
    : public iterator_type_from_frame<const bit_aligned_frame_reference<B,C,L,M>,
				      IsPlanar, IsStep, IsMutable> {};

} /* namespace sound */
} /* namespace psynth */

namespace std
{

/*
  It is important to provide an overload of uninitialized_copy for
  bit_aligned_frame_iterator. The default STL implementation calls
  placement new, which is not defined for bit_aligned_frame_iterator.
*/ 
template <typename NonAlignedFrameReference>
psynth::sound::bit_aligned_frame_iterator<NonAlignedFrameReference>
uninitialized_copy (
    psynth::sound::bit_aligned_frame_iterator<NonAlignedFrameReference> first,
    psynth::sound::bit_aligned_frame_iterator<NonAlignedFrameReference> last,
    psynth::sound::bit_aligned_frame_iterator<NonAlignedFrameReference> dst)
{
    return std::copy (first, last, dst);
}

} /* namespace std */

#endif /* PSYNTH_SOUND_BIT_ALIGNED_FRAME_ITERATOR */
