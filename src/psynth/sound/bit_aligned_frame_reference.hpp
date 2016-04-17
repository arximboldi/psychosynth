/**
 *  Time-stamp:  <2010-11-05 12:09:05 raskolnikov>
 *
 *  @file        bit_aligned_frame_reference.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 18:38:30 2010
 *
 *  Bit aligned frame reference.
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

#ifndef PSYNTH_SOUND_BIT_ALIGNED_PIXEL_REFERENCE_HPP
#define PSYNTH_SOUND_BIT_ALIGNED_PIXEL_REFERENCE_HPP

#include <functional>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/frame.hpp>
#include <psynth/sound/sample.hpp>

namespace psynth
{
namespace sound
{

/**
   bit_range

   Represents a range of bits that can span multiple consecutive
   bytes. The range has a size fixed at compile time, but the offset is
   specified at run time.

   @todo Maybe this should be in psynth::base?
*/
template <int RangeSize, bool Mutable>
class bit_range
{
public:
    typedef typename boost::mpl::if_c<Mutable,
			       unsigned char,
			       const unsigned char>::type byte_type;

    typedef std::ptrdiff_t difference_type;
    template <int RS, bool M> friend class bit_range;

private:
    byte_type* _current_byte;   // the starting byte of the bit range
    int        _bit_offset;     // offset from the beginning of the
				// current byte. 0<=_bit_offset<=7

public:
    bit_range ()
	: _current_byte (0)
	, _bit_offset (0)
    {}

    bit_range (byte_type* current_byte, int bit_offset)
	: _current_byte (current_byte)
	, _bit_offset(bit_offset)
    { assert (bit_offset >= 0 && bit_offset < 8); }

    bit_range (const bit_range& br)
	: _current_byte (br._current_byte)
	, _bit_offset (br._bit_offset)
    {}

    template <bool M>
    bit_range (const bit_range<RangeSize, M>& br)
	: _current_byte(br._current_byte)
	, _bit_offset (br._bit_offset)
    {}

    bit_range& operator= (const bit_range& br)
    {
	_current_byte = br._current_byte;
	_bit_offset   = br._bit_offset;
	return *this;
    }

    bool operator== (const bit_range& br) const
    {
	return
	    _current_byte == br._current_byte &&
	    _bit_offset == br._bit_offset;
    }

    bit_range& operator++ ()
    {
        _current_byte += (_bit_offset + RangeSize) / 8;
        _bit_offset    = (_bit_offset + RangeSize) % 8;
        return *this;
    }

    bit_range& operator-- ()
    {
	bit_advance (- RangeSize);
	return *this;
    }

    void bit_advance (difference_type num_bits)
    {
        int new_offset = int (_bit_offset + num_bits);
        _current_byte += new_offset / 8;
        _bit_offset    = new_offset % 8;
        if (_bit_offset<0) {
            _bit_offset+=8;
            --_current_byte;
        }
    }

    difference_type bit_distance_to (const bit_range& b) const
    {
        return
	    (b.current_byte() - current_byte()) * 8 +
	    b.bit_offset() - bit_offset();
    }

    byte_type* current_byte () const
    {
	return _current_byte;
    }

    int bit_offset () const
    {
	return _bit_offset;
    }
};

/**
   \defgroup ChannelBaseModelNonAlignedFrame bit_aligned_frame_reference
   \ingroup ChannelBaseModel

   \brief A heterogeneous channel base representing frame that may not
   be byte aligned, i.e. it may correspond to a bit range that does
   not start/end at a byte boundary. Models ChannelBaseConcept.
*/

/**
   \defgroup FrameModelNonAlignedFrame bit_aligned_frame_reference
   \ingroup FrameModel
   \brief A heterogeneous frame reference used to represent
   non-byte-aligned frames. Models FrameConcept

   Example:
   \code
   unsigned char data=0;

   // A mutable reference to a 6-bit BGR frame in "123" format (1 bit
   // for red, 2 bits for green, 3 bits for blue)

   typedef const bit_aligned_frame_reference<
       unsigned char, boost::mpl::vector3_c<int,1,2,3>, rgb_layout, true>
   rgb123_ref_t;

   // create the frame reference at bit offset 2
   // (i.e. red = [2], green = [3,4], blue = [5,6,7] bits)
   rgb123_ref_t ref(&data, 2);
   get_channel(ref, red_t()) = 1;
   assert(data == 0x04);
   get_channel(ref, green_t()) = 3;
   assert(data == 0x1C);
   get_channel(ref, blue_t()) = 7;
   assert(data == 0xFC);
   \endcode
*/

/**
   \ingroup ChannelBaseModelNonAlignedFrame FrameModelNonAlignedFrame
   FrameBasedModel

   \brief Heterogeneous frame reference corresponding to
   non-byte-aligned bit range. Models ChannelBaseConcept,
   FrameConcept, FrameBasedConcept
*/
template <typename BitField,
          typename SampleBitSizes,
	  // MPL integral vector defining the number of bits for each
	  // sample. For example, for 565RGB, vector_c<int, 5, 6, 5>
          typename Layout,
          bool IsMutable>
struct bit_aligned_frame_reference
{
    BOOST_STATIC_CONSTANT(
	int, bit_size = (boost::mpl::accumulate<SampleBitSizes, boost::mpl::int_<0>,
					 boost::mpl::plus<boost::mpl::_1, boost::mpl::_2>
					 >::type::value));

    typedef psynth::sound::bit_range<bit_size, IsMutable> bit_range_type;
    typedef BitField  bitfield_type;
    typedef typename boost::mpl::if_c<IsMutable,
			       unsigned char*,
			       const unsigned char*>::type data_ptr_type;
    typedef Layout layout;

    typedef typename packed_frame_type<
	bitfield_type, SampleBitSizes,Layout>::type value_type;
    typedef const bit_aligned_frame_reference reference;
    typedef const bit_aligned_frame_reference<
	BitField, SampleBitSizes, Layout, false> const_reference;

    BOOST_STATIC_CONSTANT(bool, is_mutable = IsMutable);

    bit_aligned_frame_reference () {}

    bit_aligned_frame_reference (data_ptr_type data_ptr, int bit_offset)
	: _bit_range (data_ptr, bit_offset)
    {}

    explicit bit_aligned_frame_reference (const bit_range_type& bit_range)
	: _bit_range (bit_range)
    {}

    template <bool IsMutable2> bit_aligned_frame_reference (
	const bit_aligned_frame_reference<
	    BitField, SampleBitSizes, Layout, IsMutable2>& p)
	: _bit_range(p._bit_range)
    {}

    // Mono references can be constructed from the sample reference
    explicit bit_aligned_frame_reference (
	const typename kth_element_type<
	    bit_aligned_frame_reference, 0>::type sample0)
	: _bit_range (static_cast<data_ptr_type>(&sample0), sample0.first_bit())
    {
        BOOST_STATIC_ASSERT((num_samples<
				 bit_aligned_frame_reference>::value == 1));
    }

    // Construct from another compatible frame type
    bit_aligned_frame_reference (const bit_aligned_frame_reference& p)
	: _bit_range(p._bit_range)
    {}

    template <typename BF, typename CR>
    bit_aligned_frame_reference (packed_frame<BF,CR,Layout>& p)
	: _bit_range (static_cast<data_ptr_type>(&sound::at_c<0>( p)),
		      sound::at_c<0> (p).first_bit ())
    {
        check_compatible<packed_frame<BF,CR,Layout> >();
    }

    const bit_aligned_frame_reference& operator= (
	const bit_aligned_frame_reference& p) const
    {
	static_copy (p,*this);
	return *this;
    }

    template <typename P>
    const bit_aligned_frame_reference& operator= (const P& p) const
    {
	assign (p, boost::mpl::bool_<is_frame<P>::value>());
	return *this;
    }

    template <typename P>
    bool operator== (const P& p) const
    {
	return equal (p, boost::mpl::bool_<is_frame<P>::value>());
    }

    template <typename P>
    bool operator!= (const P& p) const
    { return !(*this == p); }

    const bit_aligned_frame_reference* operator->() const
    { return this; }

    const bit_range_type& bit_range() const
    { return _bit_range; }

private:
    mutable bit_range_type _bit_range;

    template <typename B, typename C, typename L, bool M>
    friend struct bit_aligned_frame_reference;

    template <typename Frame>
    static void check_compatible()
    {
	base::psynth_function_requires<FramesCompatibleConcept<
	    Frame, bit_aligned_frame_reference> >();
    }

    template <typename Frame>
    void assign (const Frame& p, boost::mpl::true_) const
    {
	check_compatible<Frame>();
	static_copy (p, *this);
    }

    template <typename Frame>
    bool equal (const Frame& p, boost::mpl::true_) const
    {
	check_compatible<Frame>();
	return static_equal (*this, p);
    }

    static void check_mono ()
    {
	BOOST_STATIC_ASSERT((boost::is_same<
			     typename Layout::channel_space_type,
			     mono_space>::value));
    }

    template <typename Sample>
    void assign (const Sample& chan, boost::mpl::false_) const
    {
	check_mono ();
	sound::at_c<0>(*this) = chan;
    }

    template <typename Sample>
    bool equal (const Sample& chan, boost::mpl::false_) const
    {
	check_mono ();
	return sound::at_c<0>(*this)==chan;
    }
};

/*
 *
 *    ChannelBasedConcept
 *
 */

template <typename BitField, typename SampleBitSizes,
	  typename L, bool IsMutable, int K>
struct kth_element_type<bit_aligned_frame_reference<
			    BitField,SampleBitSizes,L,IsMutable>, K>
{
public:
    typedef const packed_dynamic_sample_reference<
    BitField, boost::mpl::at_c<SampleBitSizes, K>::type::value, IsMutable> type;
};

template <typename B, typename C, typename L, bool M, int K>
struct kth_element_reference_type<bit_aligned_frame_reference<B,C,L,M>, K>
    : public kth_element_type<bit_aligned_frame_reference<B,C,L,M>, K> {};

template <typename B, typename C, typename L, bool M, int K>
struct kth_element_const_reference_type<bit_aligned_frame_reference<B,C,L,M>, K>
    : public kth_element_type<bit_aligned_frame_reference<B,C,L,M>, K> {};


namespace detail
{

// returns sum of IntegralVector[0] ... IntegralVector[K-1]
template <typename IntegralVector, int K>
struct sum_k :
	public boost::mpl::plus<sum_k<IntegralVector, K-1>,
			 typename boost::mpl::at_c<IntegralVector, K-1>::type > {};

template <typename IntegralVector>
struct sum_k<IntegralVector, 0> : public boost::mpl::int_<0> {};

} /* namespace detail */

/** at_c required by MutableChannelBaseConcept */
template <int K, typename BitField, typename SampleBitSizes,
	  typename L, bool Mutable> inline
typename kth_element_reference_type<
    bit_aligned_frame_reference<BitField,SampleBitSizes,L,Mutable>,K>::type
at_c (const bit_aligned_frame_reference<BitField,SampleBitSizes,L,Mutable>& p)
{
    typedef bit_aligned_frame_reference<BitField,SampleBitSizes,L,Mutable> frame_t;
    typedef typename kth_element_reference_type<frame_t,K>::type sample_t;
    typedef typename frame_t::bit_range_type bit_range_type;

    bit_range_type bit_range (p.bit_range());
    bit_range.bit_advance (detail::sum_k<SampleBitSizes, K>::value);

    return sample_t (bit_range.current_byte(), bit_range.bit_offset ());
}

/*
 *
 *    FrameConcept
 *
 */

/**
   Metafunction predicate that flags bit_aligned_frame_reference as a
   model of FrameConcept. Required by FrameConcept
*/
template <typename B, typename C, typename L, bool M>
struct is_frame<bit_aligned_frame_reference<B,C,L,M> > : public boost::mpl::true_{};

/*
 *
 *    FrameBasedConcept
 *
 */

template <typename B, typename C, typename L, bool M>
struct channel_space_type<bit_aligned_frame_reference<B,C,L,M> >
{
    typedef typename L::channel_space type;
};

template <typename B, typename C, typename L, bool M>
struct sample_mapping_type<bit_aligned_frame_reference<B,C,L,M> >
{
    typedef typename L::sample_mapping type;
};

template <typename B, typename C, typename L, bool M>
struct is_planar<bit_aligned_frame_reference<B,C,L,M> > : boost::mpl::false_ {};


/*
 *
 *    frame_reference_type
 *
 */

namespace detail
{

// returns a vector containing K copies of the type T
template <unsigned K, typename T>
struct k_copies;

template <typename T> struct k_copies<0,T>
{
    typedef boost::mpl::vector0<> type;
};

template <unsigned K, typename T>
struct k_copies : public boost::mpl::push_back<typename k_copies<K-1,T>::type, T> {};

} /* namespace detail */

/*
  Constructs a homogeneous bit_aligned_frame_reference given a sample
  reference
*/
template <typename BitField, int NumBits, typename Layout>
struct frame_reference_type<const packed_dynamic_sample_reference<
				BitField,NumBits,false>, Layout, false, false>
{
private:
    typedef typename boost::mpl::size<typename Layout::channel_space>::type
    size_type;

    typedef typename detail::k_copies<
	size_type::value, boost::mpl::integral_c<unsigned,NumBits> >::type
    sample_bit_sizes_type;

public:
    typedef bit_aligned_frame_reference<BitField, sample_bit_sizes_type,
					Layout, false> type;
};


/*
  Same but for the mutable case. We cannot combine the mutable and
  read-only cases because this triggers ambiguity
*/
template <typename BitField, int NumBits, typename Layout>
struct frame_reference_type<const packed_dynamic_sample_reference<
				BitField,NumBits,true>, Layout, false, true>
{
private:
    typedef typename boost::mpl::size<typename Layout::channel_space>::type
    size_type;

    typedef typename detail::k_copies<
	size_type::value, boost::mpl::integral_c<unsigned, NumBits> >::type
    sample_bit_sizes_type;

public:
    typedef bit_aligned_frame_reference<
    BitField, sample_bit_sizes_type, Layout, true> type;
};

} /* namespace sound */
} /* namespace psynth */

namespace std
{

/*
  We are forced to define swap inside std namespace because on some
  platforms (Visual Studio 8) STL calls swap qualified.

  swap with 'left bias':
  - swap between proxy and anything
  - swap between value type and proxy
  - swap between proxy and proxy

  Having three overloads allows us to swap between different (but
  compatible) models of FrameConcept
*/
template <typename B, typename C, typename L, typename R> inline
void swap (const psynth::sound::bit_aligned_frame_reference<B,C,L,true> x, R& y)
{
    psynth::sound::swap_proxy<
	typename psynth::sound::bit_aligned_frame_reference<
	    B,C,L,true>::value_type>(x, y);
}


template <typename B, typename C, typename L> inline
void swap (typename psynth::sound::bit_aligned_frame_reference<
	       B,C,L,true>::value_type& x,
	   const psynth::sound::bit_aligned_frame_reference<B,C,L,true> y)
{
    psynth::sound::swap_proxy<
	typename psynth::sound::bit_aligned_frame_reference<
	    B,C,L,true>::value_type>(x, y);
}

template <typename B, typename C, typename L> inline
void swap (const psynth::sound::bit_aligned_frame_reference<B,C,L,true> x,
	   const psynth::sound::bit_aligned_frame_reference<B,C,L,true> y)
{
    psynth::sound::swap_proxy<
	typename psynth::sound::bit_aligned_frame_reference<
	    B,C,L,true>::value_type> (x, y);
}

} /* namespace std */

#endif /* PSYNTH_SOUND_BIT_ALIGNED_FRAME_REFERENCE */
