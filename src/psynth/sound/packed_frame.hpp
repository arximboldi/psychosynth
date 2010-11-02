/**
 *  Time-stamp:  <2010-11-02 11:01:40 raskolnikov>
 *
 *  @file        packed_frame.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 11:36:15 2010
 *
 *  Packed frame.
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

#ifndef PSYNTH_SOUND_PACKED_FRAME_H
#define PSYNTH_SOUND_PACKED_FRAME_H

#include <functional>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/front.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/frame.hpp>

namespace psynth
{
namespace sound
{

/**
   \defgroup ChannelBaseModelPackedFrame packed_frame 
   \ingroup ChannelBaseModel

   \brief A heterogeneous channel base whose elements are reference
   proxies to samples in a frame. Models ChannelBaseValueConcept. This
   class is used to model packed frames, such as 16-bit packed RGB.
*/

/**
   \defgroup FrameModelPackedFrame packed_frame 
   \ingroup FrameModel

   \brief A heterogeneous frame used to represent packed frames with
   non-byte-aligned samples. Models FrameValueConcept

   Example:
   \code
   typedef packed_frame_type<uint16_t,
                             mpl::vector3_c<unsigned,5,6,5>,
			     rgb_layout>::type rgb565_frame_t;
   BOOST_STATIC_ASSERT((sizeof(rgb565_frame_t)==2));

   rgb565_frame_t r565;
   get_channel(r565,red_t())   = 31;
   get_channel(r565,green_t()) = 63;
   get_channel(r565,blue_t())  = 31;
   assert(r565 == rgb565_frame_t((uint16_t)0xFFFF));    
\endcode
*/

/**
   \ingroup ChannelBaseModelPackedFrame FrameModelPackedFrame FrameBasedModel

   \brief Heterogeneous frame value whose sample references can be
   constructed from the frame bitfield and their index. Models
   ChannelBaseValueConcept, FrameValueConcept, FrameBasedConcept
   
   Typical use for this is a model of a packed frame (like 565 RGB)
*/

template <typename BitField,
	  // A type that holds the bits of the frame. Typically an
	  // integral type, like boost::uint16_t
          typename SampleRefVec,
	  // An MPL vector whose elements are packed samples. They
	  // must be constructible from BitField. GIL uses
	  // packed_sample_reference
          typename Layout
	  // Layout defining the channel space and ordering of the
	  // samples. Example value: rgb_layout
	  >
struct packed_frame
{
    BitField _bitfield;

    typedef Layout                layout;
    typedef packed_frame          value_type;
    typedef value_type&           reference;
    typedef const value_type&     const_reference;

    BOOST_STATIC_CONSTANT(bool, is_mutable = sample_traits<
			      typename mpl::front<SampleRefVec>::type
			      >::is_mutable);

    packed_frame () {}
    
    explicit packed_frame (const BitField& bitfield)
	: _bitfield(bitfield) {}

    // Construct from another compatible frame type
    packed_frame (const packed_frame& p)
	: _bitfield (p._bitfield) {}
    
    template <typename P>
    packed_frame (const P& p,
		  typename enable_if_c<is_frame<P>::value>::type* d=0)
    {
	check_compatible<P> ();
	static_copy (p, *this);
    }
    
    packed_frame (int chan0, int chan1)
	: _bitfield(0)
    { 
        BOOST_STATIC_ASSERT((num_samples<packed_frame>::value == 2)); 
        at_c<0>(*this) = chan0;
	at_c<1>(*this) = chan1; 
    }
    
    packed_frame (int chan0, int chan1, int chan2)
	: _bitfield(0)
    { 
        BOOST_STATIC_ASSERT((num_samples<packed_frame>::value == 3)); 
        at_c<0>(*this) = chan0;
	at_c<1>(*this) = chan1;
	at_c<2>(*this) = chan2; 
    }
    
    packed_frame(int chan0, int chan1, int chan2, int chan3)
	: _bitfield (0)
    { 
        BOOST_STATIC_ASSERT((num_samples<packed_frame>::value==4)); 
        at_c<0>(*this) = chan0;
	at_c<1>(*this) = chan1;
	at_c<2>(*this) = chan2;
	at_c<3>(*this) = chan3; 
    }
    
    packed_frame(int chan0, int chan1, int chan2, int chan3, int chan4)
	: _bitfield(0)
    { 
        BOOST_STATIC_ASSERT((num_samples<packed_frame>::value == 5)); 
        at_c<0>(*this) = chan0;
	at_c<1>(*this) = chan1;
	at_c<2>(*this) = chan2;
	at_c<3>(*this) = chan3;
	at_c<4>(*this) = chan4;
    } 

    packed_frame& operator= (const packed_frame& p)
    {
	_bitfield = p._bitfield;
	return *this;
    }

    template <typename P>
    packed_frame& operator= (const P& p)
    {
	assign (p, mpl::bool_<is_frame<P>::value>());
	return *this;
    }
    
    template <typename P>
    bool operator== (const P& p) const
    {
	return equal (p, mpl::bool_<is_frame<P>::value>());
    } 

    template <typename P>
    bool operator!= (const P& p) const
    {
	return !(*this==p);
    }

    packed_frame& operator= (int chan)
    {
	check_gray ();
	at_c<0>(*this) = chan;
	return *this;
    }
    
    bool operator== (int chan) const
    {
	check_gray ();
	return at_c<0>(*this) == chan;
    }
    
private:    
    template <typename Frame>
    static void check_compatible()
    {
	psynth_function_requires<
	    FramesCompatibleConcept<Frame, packed_frame> >();
    }
    
    template <typename Frame>
    void assign(const Frame& p, mpl::true_)
    {
	check_compatible<Frame>();
	static_copy (p,*this);
    }
    
    template <typename Frame>
    bool  equal (const Frame& p, mpl::true_) const
    {
	check_compatible<Frame>();
	return static_equal(*this,p);
    } 

    // Support for assignment/equality comparison of a sample with a
    // grayscale frame
    static void check_gray()
    {
	BOOST_STATIC_ASSERT((is_same<typename Layout::channel_space_t,
			     gray_t>::value));
    }
    
    template <typename Sample>
    void assign (const Sample& chan, mpl::false_)
    {
	check_gray ();
	at_c<0>(*this) = chan;
    }
    
    template <typename Sample>
    bool equal (const Sample& chan, mpl::false_) const
    {
	check_gray ();
	return at_c<0>(*this) == chan;
    }
};


/*
 *
 *   ChannelBasedConcept
 *
 */

template <typename BitField, typename SampleRefVec, typename Layout, int K>  
struct kth_element_type<packed_frame<BitField,SampleRefVec,Layout>,K> :
	public mpl::at_c<SampleRefVec, K> {};

template <typename BitField, typename SampleRefVec, typename Layout, int K>  
struct kth_element_reference_type<packed_frame<BitField,SampleRefVec,Layout>,K> :
	public mpl::at_c<SampleRefVec, K> {};

template <typename BitField, typename SampleRefVec, typename Layout, int K>  
struct kth_element_const_reference_type<
    packed_frame<BitField, SampleRefVec, Layout>, K>
{
    typedef typename sample_traits<
	typename mpl::at_c<SampleRefVec, K>::type>::const_reference type;
};

template <int K, typename P, typename C, typename L> inline
typename kth_element_reference_type<packed_frame<P,C,L>, K>::type 
at_c (packed_frame<P,C,L>& p)
{
    return typename kth_element_reference_type<
	packed_frame<P,C,L>, K>::type (&p._bitfield); 
}

template <int K, typename P, typename C, typename L> inline
typename kth_element_const_reference_type<packed_frame<P,C,L>, K>::type 
at_c (const packed_frame<P,C,L>& p)
{ 
    return typename kth_element_const_reference_type<
	packed_frame<P,C,L>, K>::type (&p._bitfield);
}


/*
 *
 *    FrameConcept
 *
 */

/* Metafunction predicate that flags packed_frame as a model of
 * FrameConcept. Required by FrameConcept */
template <typename BitField, typename SampleRefVec, typename Layout>  
struct is_frame<packed_frame<BitField, SampleRefVec, Layout> > :
    public mpl::true_ {};


/*
 *
 *    FrameBasedConcept
 *
 */

template <typename P, typename C, typename Layout>
struct channel_space_type<packed_frame<P,C,Layout> > {
    typedef typename Layout::channel_space_t type;
}; 

template <typename P, typename C, typename Layout>
struct sample_mapping_type<packed_frame<P,C,Layout> > {
    typedef typename Layout::sample_mapping_t type;
}; 

template <typename P, typename C, typename Layout>
struct is_planar<packed_frame<P,C,Layout> > : mpl::false_ {}; 


/*
 *
 *    Support for interleaved iterators over packed frame
 *
 */

/**
  \defgroup FrameIteratorModelPackedInterleavedPtr Pointer to
  packed_frame<P,CR,Layout>
  
  \ingroup FrameIteratorModel
  \brief Iterators over interleaved frames.

  The pointer packed_frame<P,CR,Layout>* is used as an iterator over
  interleaved frames of packed format. Models FrameIteratorConcept,
  HasDynamicXStepTypeConcept, MemoryBasedIteratorConcept
*/
template <typename P, typename C, typename L>  
struct iterator_is_mutable<packed_frame<P,C,L>*> :
    public mpl::bool_<packed_frame<P,C,L>::is_mutable> {};

template <typename P, typename C, typename L>  
struct iterator_is_mutable<const packed_frame<P,C,L>*> :
    public mpl::false_ {};

} /* namespace sound */
} /* namespace psynth */

namespace boost
{

template <typename P, typename C, typename L>
struct has_trivial_constructor<psynth::sound::packed_frame<P,C,L> > :
	public has_trivial_constructor<P> {};

} /* namespace boost */

#endif /* PSYNTH_PACKED_FRAME_H_ */
