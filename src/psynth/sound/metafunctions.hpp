/**
 *  Time-stamp:  <2010-11-05 14:57:02 raskolnikov>
 *
 *  @file        metafunctions.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 21:04:05 2010
 *
 *  Cool metafunctions!
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

#ifndef PSYNTH_SOUND_METAFUNCTIONS_HPP
#define PSYNTH_SOUND_METAFUNCTIONS_HPP

#include <iterator>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/print.hpp>
#include <boost/type_traits.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/sample.hpp>

namespace psynth
{
namespace sound
{

// forward declarations
template <typename T, typename L> struct frame;
template <typename BitField,typename SampleRefVec,typename Layout>
struct packed_frame;
template <typename T, typename C> struct planar_frame_reference;
template <typename IC, typename C> struct planar_frame_iterator;
template <typename I> class memory_based_step_iterator;
template <typename I> class memory_based_2d_locator;
template <typename L> class buffer_view;
template <typename Frame, bool IsPlanar, typename Alloc> class buffer;
template <typename T> struct sample_type;
template <typename T> struct channel_space_type;
template <typename T> struct sample_mapping_type;
template <typename It> struct is_iterator_adaptor;
template <typename It> struct iterator_adaptor_get_base;
template <typename BitField, typename SampleBitSizes, typename Layout,
	  bool IsMutable>
struct bit_aligned_frame_reference;

/*
 *
 *    TYPE ANALYSIS METAFUNCTIONS
 *    Predicate metafunctions determining properties of GIL types
 *
 */

/*
 \defgroup GILIsBasic xxx_is_basic
 \ingroup TypeAnalysis

 \brief Determines if GIL constructs are basic.  Basic constructs are
 the ones that can be generated with the type factory methods
 frame_reference_type, iterator_type, locator_type, view_type and
 buffer_type They can be mutable/immutable, planar/interleaved,
 step/nonstep. They must use GIL-provided models.

 \brief Determines if a given frame reference is basic Basic
 references must use gil::frame& (if interleaved),
 gil::planar_frame_reference (if planar). They must use the standard
 constness rules.

 \ingroup GILIsBasic
*/

template <typename FrameRef>
struct frame_reference_is_basic  : public boost::mpl::false_ {};
template <typename T,  typename L>
struct frame_reference_is_basic<frame<T, L>&> : public boost::mpl::true_ {};
template <typename T,  typename L>
struct frame_reference_is_basic<const frame<T,L>&> : public boost::mpl::true_ {};
template <typename TR, typename Cs>
struct frame_reference_is_basic<planar_frame_reference<TR,Cs> > :
	public boost::mpl::true_ {};
template <typename TR, typename Cs>
struct frame_reference_is_basic<const planar_frame_reference<TR,Cs> > :
	public boost::mpl::true_ {};


/**
   \brief Determines if a given frame iterator is basic

   Basic iterators must use gil::frame (if interleaved),
   gil::planar_frame_iterator (if planar) and
   gil::memory_based_step_iterator (if step). They must use the
   standard constness rules.

   \ingroup GILIsBasic
*/
template <typename Iterator>
struct iterator_is_basic : public boost::mpl::false_ {};
template <typename T, typename L>  // mutable   interleaved
struct iterator_is_basic<      frame<T,L>*      > : public boost::mpl::true_ {};
template <typename T, typename L>  // immutable interleaved
struct iterator_is_basic<const frame<T,L>*      > : public boost::mpl::true_ {};
template <typename T, typename Cs>  // mutable   planar
struct iterator_is_basic<planar_frame_iterator<      T*,Cs> > :
	public boost::mpl::true_ {};
template <typename T, typename Cs>    // immutable planar
struct iterator_is_basic<planar_frame_iterator<const T*,Cs> > :
	public boost::mpl::true_ {};
template <typename T, typename L>  // mutable   interleaved step
struct iterator_is_basic<memory_based_step_iterator<      frame<T,L>*> > :
	public boost::mpl::true_ {};
template <typename T, typename L>  // immutable interleaved step
struct iterator_is_basic<memory_based_step_iterator<const frame<T,L>*> > :
	public boost::mpl::true_ {};
template <typename T, typename Cs>  // mutable   planar step
struct iterator_is_basic<
    memory_based_step_iterator<planar_frame_iterator<      T*,Cs> > > :
	public boost::mpl::true_ {};
template <typename T, typename Cs>    // immutable planar step
struct iterator_is_basic<
    memory_based_step_iterator<planar_frame_iterator<const T*,Cs> > > :
	public boost::mpl::true_ {};


/**
   \ingroup GILIsBasic
   \brief Basic views must be over basic locators
*/
template <typename View>
struct view_is_basic : public boost::mpl::false_ {};
template <typename Loc>
struct view_is_basic<buffer_view<Loc> > : public iterator_is_basic<Loc> {};

/**
   \ingroup GILIsBasic
   \brief Basic buffers must use basic views and std::allocator of
   char
*/
template <typename Img>
struct buffer_is_basic : public boost::mpl::false_ {};
template <typename Frame, bool IsPlanar, typename Alloc>
struct buffer_is_basic<buffer<Frame,IsPlanar,Alloc> > : public boost::mpl::true_ {};


/**
   \defgroup GILIsStep xxx_is_step
   \ingroup TypeAnalysis
   \brief Determines if the given iterator/locator/view has a step
   that could be set dynamically
*/

template <typename I> struct iterator_is_step;

namespace detail
{

template <typename It, bool IsBase, bool EqualsStepType>
struct iterator_is_step_impl;

// iterator that has the same type as its dynamic_step_type must be
// a step iterator
template <typename It, bool IsBase>
struct iterator_is_step_impl<It,IsBase,true> : public boost::mpl::true_{};

// base iterator can never be a step iterator
template <typename It>
struct iterator_is_step_impl<It,true,false> : public boost::mpl::false_{};

// for an iterator adaptor, see if its base is step
template <typename It> struct iterator_is_step_impl<It,false,false> 
    : public iterator_is_step<typename iterator_adaptor_get_base<It>::type>{};

} /* namespace detail */


/**
   \ingroup GILIsStep
   \brief Determines if the given iterator has a step that could be
   set dynamically
*/
template <typename I> struct iterator_is_step 
    : public detail::iterator_is_step_impl<
    I, 
    !is_iterator_adaptor<I>::type::value,
    boost::is_same<I,typename dynamic_step_type<I>::type>::value > {};


/**
   \ingroup GILIsStep
   \brief Determines if the given locator has a step that can be set
   dynamically.
*/
template <typename V> struct view_is_step :
	public iterator_is_step<typename V::iterator> {}; 


/**
   \brief Determines whether the given frame reference is a proxy
   class or a native C++ reference
   
   \ingroup TypeAnalysis
*/
template <typename FrameReference>
struct frame_reference_is_proxy
    : public boost::mpl::not_<
    boost::is_same<typename remove_const_and_reference<FrameReference>::type,
	    typename remove_const_and_reference<
		FrameReference>::type::value_type> > {};

/**
   \brief Given a model of a frame, determines whether the model
   represents a frame reference (as opposed to frame value)
   
   \ingroup TypeAnalysis
*/
template <typename Frame>
struct frame_is_reference :
    public boost::mpl::or_<boost::is_reference<Frame>,
		    frame_reference_is_proxy<Frame> > {};

/**
   \defgroup GILIsMutable xxx_is_mutable
   \ingroup TypeAnalysis
   / \brief Determines if the given frame
   reference/iterator/locator/view is mutable (i.e. its frames can be
   changed)
*/

/**
   \ingroup GILIsMutable
   \brief Determines if the given frame reference is mutable (i.e. its
   samples can be changed)
   
   Note that built-in C++ references obey the const qualifier but
   reference proxy classes do not.
*/
template <typename R>
struct frame_reference_is_mutable :
	public boost::mpl::bool_<boost::remove_reference<R>::type::is_mutable> {};

template <typename R>
struct frame_reference_is_mutable<const R&>
    : public boost::mpl::and_<frame_reference_is_proxy<R>,
		       frame_reference_is_mutable<R> > {};

/**
   \ingroup GILIsMutable
   \brief Determines if the given view is mutable (i.e. its frames can
   be changed)
*/
template <typename V>
struct view_is_mutable : public iterator_is_mutable<typename V::iterator> {};


/*
 *
 *    TYPE FACTORY METAFUNCTIONS
 *    Metafunctions returning GIL types from other GIL types
 *
 */

/**

   \defgroup TypeFactoryFromElements xxx_type
   \ingroup TypeFactory
   \brief Returns the type of a homogeneous GIL construct given its
   elements (sample, layout, whether it is planar, step, mutable,
   etc.)

   \defgroup TypeFactoryFromFrame xxx_type_from_frame
   \ingroup TypeFactory
   \brief Returns the type of a GIL construct given its frame type,
   whether it is planar, step, mutable, etc.

   \defgroup TypeFactoryDerived derived_xxx_type
   \ingroup TypeFactory
   \brief Returns the type of a homogeneous GIL construct given a
   related construct by changing some of its properties

   \ingroup TypeFactoryFromElements
   \brief Returns the type of a homogeneous frame reference given the
   sample type, layout, whether it operates on planar data and whether
   it is mutable

*/

template <typename T, typename L, bool IsPlanar=false, bool IsMutable=true>
struct frame_reference_type{};
template <typename T, typename L>
struct frame_reference_type<T,L,false,true > { typedef frame<T,L>& type; };
template <typename T, typename L>
struct frame_reference_type<T,L,false,false> { typedef const frame<T,L>& type; };

template <typename T, typename L>
struct frame_reference_type<T,L,true,true>
{
    typedef const planar_frame_reference<
	typename sample_traits<T>::reference,
	typename channel_space_type<L>::type> type;
};       // TODO: Assert M=identity

template <typename T, typename L>
struct frame_reference_type<T,L,true,false>
{
    typedef const planar_frame_reference<
	typename sample_traits<T>::const_reference,
	typename channel_space_type<L>::type> type;
};// TODO: Assert M=identity

/**
   \ingroup TypeFactoryFromFrame
   \brief Returns the type of a frame iterator given the frame type,
   whether it operates on planar data, whether it is a step iterator,
   and whether it is mutable
*/
template <typename Frame, bool IsPlanar=false,
	  bool IsStep=false, bool IsMutable=true>
struct iterator_type_from_frame {};

template <typename Frame>
struct iterator_type_from_frame<Frame,false,false,true >
{ typedef Frame* type; };

template <typename Frame>
struct iterator_type_from_frame<Frame,false,false,false>
{ typedef const Frame* type; };

template <typename Frame>
struct iterator_type_from_frame<Frame,true,false,true>
{ 
    typedef planar_frame_iterator<
	typename sample_traits<typename sample_type<Frame>::type>::pointer,
	typename channel_space_type<Frame>::type> type; 
};

template <typename Frame>
struct iterator_type_from_frame<Frame,true,false,false>
{ 
    typedef planar_frame_iterator<
	typename sample_traits<typename sample_type<Frame>::type>::const_pointer,
	typename channel_space_type<Frame>::type> type; 
};

template <typename Frame, bool IsPlanar, bool IsMutable>
struct iterator_type_from_frame<Frame,IsPlanar,true,IsMutable>
{ 
    typedef memory_based_step_iterator<
	typename iterator_type_from_frame<
	    Frame,IsPlanar,false,IsMutable>::type> type; 
};


/**
   \ingroup TypeFactoryFromElements
   \brief Returns the type of a homogeneous iterator given the sample
   type, layout, whether it operates on planar data, whether it is a
   step iterator, and whether it is mutable
*/
template <typename T, typename L,
	  bool IsPlanar=false, bool IsStep=false, bool IsMutable=true>
struct iterator_type {};

template <typename T, typename L>
struct iterator_type<T,L,false,false,true >
{
    typedef frame<T,L>* type;
};

template <typename T, typename L>
struct iterator_type<T,L,false,false,false>
{
    typedef const frame<T,L>* type;
};

template <typename T, typename L>
struct iterator_type<T,L,true,false,true>
{
    typedef planar_frame_iterator<T*,typename L::channel_space> type;
};
// TODO: Assert M=identity

template <typename T, typename L>
struct iterator_type<T,L,true,false,false>
{
    typedef planar_frame_iterator<const T*,typename L::channel_space> type;
};
// TODO: Assert M=identity

template <typename T, typename L, bool IsPlanar, bool IsMutable>
struct iterator_type<T,L,IsPlanar,true,IsMutable>
{ 
    typedef memory_based_step_iterator<
	typename iterator_type<T,L,IsPlanar,false,IsMutable>::type> type; 
};


/**
   \brief Given a frame iterator defining access to frames along a
   row, returns the types of the corresponding built-in step_iterator,
   xy_locator, buffer_view
   
   \ingroup TypeFactory
   @see boost::gil::type_from_x_iterator
*/   
template <typename Iterator> 
struct type_from_iterator
{
    typedef memory_based_step_iterator<Iterator>  step_iterator;
    typedef buffer_view<Iterator>                 view;
};

namespace detail
{

template <typename BitField, typename FirstBit, typename NumBits>
struct packed_sample_reference_type
{
    typedef const packed_sample_reference<
	BitField,FirstBit::value,NumBits::value,true> type;
};

template <typename BitField, typename SampleBitSizesVector>
class packed_sample_references_vector_type
{
    // If SampleBitSizesVector is boost::mpl::vector<int,7,7,2> Then
    // first_bits_vector will be boost::mpl::vector<int,0,7,14,16>
    typedef typename boost::mpl::accumulate<
	SampleBitSizesVector,
	boost::mpl::vector1<boost::mpl::int_<0> >, 
	boost::mpl::push_back<boost::mpl::_1, boost::mpl::plus<boost::mpl::back<boost::mpl::_1>, boost::mpl::_2> >
	>::type first_bits_vector;

public:
    typedef typename boost::mpl::transform<
    typename boost::mpl::pop_back<first_bits_vector>::type,
    SampleBitSizesVector,
    packed_sample_reference_type<BitField, boost::mpl::_1,boost::mpl::_2> >::type type;
};

} /* namespace detail */

/**
   \ingroup TypeFactoryFromElements
   \brief Returns the type of a packed frame given its bitfield type,
   the bit size of its samples and its layout.
   
   A packed frame has samples that cover bit ranges but itself is byte
   aligned. RGB565 frame is an example.
   
   The size of SampleBitSizeVector must equal the number of samples in
   the given layout The sum of bit sizes for all samples must be less
   than or equal to the number of bits in BitField (and cannot exceed
   64).  If it is less than the number of bits in BitField, the last
   bits will be unused.
*/
template <typename BitField, typename SampleBitSizeVector, typename Layout>
struct packed_frame_type
{
    typedef packed_frame<
	BitField,
	typename detail::packed_sample_references_vector_type<
	    BitField, SampleBitSizeVector>::type, Layout> type;
};


/**
   \defgroup TypeFactoryPacked packed_buffer_type,bit_aligned_buffer_type
   \ingroup TypeFactoryFromElements

   \brief Returns the type of an buffer whose samples are not
   byte-aligned.
 
   A packed buffer is an buffer whose frames are byte aligned, such as
   "rgb565". <br> A bit-aligned buffer is an buffer whose frames are
   not byte aligned, such as "rgb222". <br>
 
   The sum of the bit sizes of all samples cannot exceed 64.
*/

/*
  \ingroup TypeFactoryPacked
  \brief Returns the type of an interleaved packed buffer: an buffer
  whose samples may not be byte-aligned, but whose frames are byte
  aligned.
*/
template <typename BitField, typename SampleBitSizeVector,
	  typename Layout, typename Alloc=std::allocator<unsigned char> >
struct packed_buffer_type
{
    typedef buffer<
	typename packed_frame_type<BitField, SampleBitSizeVector, Layout>::type,
	false,Alloc> type;
};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a single-sample buffer given its
   bitfield type, the bit size of its sample and its layout
*/
template <typename BitField, unsigned Size1, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct packed_buffer1_type :
    public packed_buffer_type<BitField, boost::mpl::vector1_c<unsigned, Size1>,
			      Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a two sample buffer given its bitfield
   type, the bit size of its samples and its layout
*/
template <typename BitField, unsigned Size1, unsigned Size2, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct packed_buffer2_type :
    public packed_buffer_type<BitField, boost::mpl::vector2_c<unsigned, Size1, Size2>,
			      Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a three sample buffer given its bitfield
   type, the bit size of its samples and its layout
*/
template <typename BitField, unsigned Size1, unsigned Size2,
	  unsigned Size3, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct packed_buffer3_type :
    public packed_buffer_type<
    BitField, boost::mpl::vector3_c<unsigned, Size1, Size2, Size3>, Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a four sample buffer given its bitfield
   type, the bit size of its samples and its layout
*/
template <typename BitField, unsigned Size1, unsigned Size2,
	  unsigned Size3, unsigned Size4, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct packed_buffer4_type :
    public packed_buffer_type<
    BitField,
    boost::mpl::vector4_c<unsigned, Size1, Size2, Size3, Size4>,
    Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a five sample buffer given its bitfield
   type, the bit size of its samples and its layout
*/
template <typename BitField, unsigned Size1, unsigned Size2, unsigned Size3,
	  unsigned Size4, unsigned Size5, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct packed_buffer5_type :
    public packed_buffer_type<
    BitField, boost::mpl::vector5_c<
		  unsigned, Size1, Size2, Size3, Size4, Size5>,
    Layout, Alloc> {};


/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a packed buffer whose frames may not be
   byte aligned. For example, an "rgb222" buffer is bit-aligned
   because its frame spans six bits.

   Note that the alignment parameter in the constructor of bit-aligned
   buffers is in bit units. For example, if you want to construct a
   bit-aligned buffer whose rows are byte-aligned, use 8 as the
   alignment parameter, not 1.
*/
template <typename SampleBitSizeVector, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer_type
{
private:
    BOOST_STATIC_CONSTANT(
	int,
	bit_size = (boost::mpl::accumulate<
			SampleBitSizeVector,
			boost::mpl::int_<0>,
			boost::mpl::plus<boost::mpl::_1,
					 boost::mpl::_2> >::type::value));
    typedef typename detail::min_fast_uint<
	bit_size + 7>::type bitfield_t;  
    typedef const bit_aligned_frame_reference<
	bitfield_t, SampleBitSizeVector, Layout, true> bit_alignedref_t;

public:
    typedef buffer<bit_alignedref_t,false,Alloc> type;
};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a single-sample bit-aligned buffer given
   the bit size of its sample and its layout
*/
template <unsigned Size1, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer1_type :
    public bit_aligned_buffer_type<boost::mpl::vector1_c<unsigned, Size1>,
				   Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a two sample bit-aligned buffer given
   the bit size of its samples and its layout
*/
template <unsigned Size1, unsigned Size2, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer2_type :
    public bit_aligned_buffer_type<
    boost::mpl::vector2_c<unsigned, Size1, Size2>, Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a three sample bit-aligned buffer given
   the bit size of its samples and its layout
*/
template <unsigned Size1, unsigned Size2, unsigned Size3,
	  typename Layout, typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer3_type :
    public bit_aligned_buffer_type<
    boost::mpl::vector3_c<unsigned, Size1, Size2, Size3>, Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a four sample bit-aligned buffer given
   the bit size of its samples and its layout
*/
template <unsigned Size1, unsigned Size2, unsigned Size3, unsigned Size4,
	  typename Layout, typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer4_type :
    public bit_aligned_buffer_type<
    boost::mpl::vector4_c<unsigned, Size1, Size2, Size3, Size4>, Layout, Alloc> {};

/**
   \ingroup TypeFactoryPacked
   \brief Returns the type of a five sample bit-aligned buffer given
   the bit size of its samples and its layout
*/
template <unsigned Size1, unsigned Size2, unsigned Size3, unsigned Size4,
	  unsigned Size5, typename Layout,
	  typename Alloc=std::allocator<unsigned char> >
struct bit_aligned_buffer5_type :
    public bit_aligned_buffer_type<
    boost::mpl::vector5_c<unsigned, Size1, Size2, Size3, Size4, Size5>,
    Layout, Alloc> {};



/**
   \ingroup TypeFactoryFromElements
   \brief Returns the type of a homogeneous frame given the sample
   type and layout
*/
template <typename Sample, typename Layout> 
struct frame_value_type
{
    typedef frame<Sample,Layout> type;
    // by default use gil::frame. Specializations are provided for 
};

// Specializations for packed samples
template <typename BitField, int NumBits, bool IsMutable, typename Layout> 
struct frame_value_type<packed_dynamic_sample_reference<
			    BitField,NumBits,IsMutable>,Layout> :
    public packed_frame_type<BitField,
			     boost::mpl::vector1_c<unsigned,NumBits>, Layout> {};

template <typename BitField, int NumBits, bool IsMutable, typename Layout> 
struct frame_value_type<const packed_dynamic_sample_reference<
			    BitField,NumBits,IsMutable>,Layout> :
    public packed_frame_type<BitField,
			     boost::mpl::vector1_c<unsigned,NumBits>, Layout> {};

template <typename BitField, int FirstBit, int NumBits, bool IsMutable,
	  typename Layout> 
struct frame_value_type<packed_sample_reference<
			    BitField,FirstBit,NumBits,IsMutable>,Layout> :
    public packed_frame_type<BitField,
			     boost::mpl::vector1_c<unsigned,NumBits>, Layout> {};

template <typename BitField, int FirstBit, int NumBits, bool IsMutable,
	  typename Layout> 
struct frame_value_type<const packed_sample_reference<
			    BitField,FirstBit,NumBits,IsMutable>,Layout> :
    public packed_frame_type<BitField,
			     boost::mpl::vector1_c<unsigned,NumBits>, Layout> {};

template <int NumBits, typename Layout> 
struct frame_value_type<packed_sample_value<NumBits>,Layout> :
    public packed_frame_type<typename detail::min_fast_uint<NumBits>::type,
			     boost::mpl::vector1_c<unsigned,NumBits>, Layout> {};


/**
   \ingroup TypeFactoryFromElements
   \brief Returns the type of a homogeneous view given the sample
   type, layout, whether it operates on planar data and whether it has
   a step horizontally
*/
template <typename T, typename L, bool IsPlanar=false,
	  bool IsStepX=false, bool IsMutable=true> 
struct view_type
{
    typedef typename type_from_iterator<
	typename iterator_type<
	    T,L,IsPlanar,IsStepX,IsMutable>::type>::view type;
};

/**
   \ingroup TypeFactoryFromElements
   \brief Returns the type of a homogeneous buffer given the sample
   type, layout, and whether it operates on planar data
*/
template <typename T, typename L, bool IsPlanar=false,
	  typename Alloc=std::allocator<unsigned char> > 
struct buffer_type
{
    typedef buffer<frame<T,L>, IsPlanar, Alloc> type;
};

/**
   \ingroup TypeFactoryFromFrame
   \brief Returns the type of a view the frame type, whether it
   operates on planar data and whether it has a step horizontally
*/
template <typename Frame, bool IsPlanar=false, bool IsStepX=false,
	  bool IsMutable=true> 
struct view_type_from_frame
{
    typedef typename type_from_iterator<
	typename iterator_type_from_frame<
	    Frame,IsPlanar,IsStepX,IsMutable>::type>::view type;
};


/**
   \brief Constructs a frame reference type from a source frame
   reference type by changing some of the properties.
   
   \ingroup TypeFactoryDerived
   Use use_default for the properties of the source view that you want
   to keep
*/
template <typename Ref,
	  typename T=boost::use_default,
	  typename L=boost::use_default,
	  typename IsPlanar=boost::use_default,
	  typename IsMutable=boost::use_default>
class derived_frame_reference_type
{
    typedef typename boost::remove_reference<Ref>::type frame_t;

    typedef typename  boost::mpl::if_<boost::is_same<T, boost::use_default>,
			       typename sample_type<frame_t>::type,
			       T>::type sample_type;
    
    typedef typename  boost::mpl::if_<
	boost::is_same<L, boost::use_default>, 
	layout<typename channel_space_type<frame_t>::type,
	       typename sample_mapping_type<frame_t>::type>, L>::type layout_type;

    static const bool mut = boost::mpl::if_<
	boost::is_same<IsMutable,boost::use_default>,
	frame_reference_is_mutable<Ref>,
	IsMutable>::type::value;

    static const bool planar = boost::mpl::if_<
	boost::is_same<IsPlanar,boost::use_default>,
	is_planar<frame_t>,
	IsPlanar>::type::value;

public:
    typedef typename frame_reference_type<
    sample_type, layout_type, planar, mut>::type type;
};


/**
   \brief Constructs a frame iterator type from a source frame
   iterator type by changing some of the properties.
   
   \ingroup TypeFactoryDerived
   Use boost::use_default for the properties of the source view that you want
   to keep
*/
template <typename Iterator,
	  typename T=boost::use_default,
	  typename L=boost::use_default,
	  typename IsPlanar=boost::use_default,
	  typename IsStep=boost::use_default,
	  typename IsMutable=boost::use_default>
class derived_iterator_type
{
    typedef typename  boost::mpl::if_<
	boost::is_same<T ,boost::use_default>,
	typename sample_type<Iterator>::type, T >::type sample_t;

    typedef typename  boost::mpl::if_<
	boost::is_same<L,boost::use_default>, 
	layout<typename channel_space_type<Iterator>::type,
	       typename sample_mapping_type<Iterator>::type>, L>::type layout_t;

    static const bool mut    = boost::mpl::if_<
	boost::is_same<IsMutable, boost::use_default>,
	iterator_is_mutable<Iterator>,
	IsMutable>::type::value;

    static const bool planar = boost::mpl::if_<
	boost::is_same<IsPlanar, boost::use_default>,
	is_planar<Iterator>,
	IsPlanar>::type::value;

    static const bool step   = boost::mpl::if_<
	boost::is_same<IsStep, boost::use_default>,
	iterator_is_step<Iterator>,
	IsStep>::type::value;
    
public:
    typedef typename iterator_type<
    sample_t, layout_t, planar, step, mut>::type type;
};


/**
   \brief Constructs an buffer view type from a source view type by
   changing some of the properties.
   
   \ingroup TypeFactoryDerived
   Use boost::use_default for the properties of the source view that you want
   to keep
*/
template <typename View, typename T=boost::use_default,
	  typename L=boost::use_default,
	  typename IsPlanar=boost::use_default,
	  typename StepX=boost::use_default,
	  typename IsMutable=boost::use_default>
class derived_view_type
{
    typedef typename  boost::mpl::if_<
	boost::is_same<T ,boost::use_default>,
	typename sample_type<View>::type,
	T>::type sample_t;

    typedef typename  boost::mpl::if_<
	boost::is_same<L,boost::use_default>, 
	layout<typename channel_space_type<View>::type,
	       typename sample_mapping_type<View>::type>,
	L>::type layout_t;

    static const bool mut    = boost::mpl::if_<
	boost::is_same<IsMutable,boost::use_default>,
	view_is_mutable<View>,
	IsMutable>::type::value;

    static const bool planar = boost::mpl::if_<
	boost::is_same<IsPlanar,boost::use_default>,
	is_planar<View>,
	IsPlanar>::type::value;
    
    static const bool step   = boost::mpl::if_<
	boost::is_same<StepX ,boost::use_default>,
	view_is_step<View>, StepX>::type::value;

public:
    typedef typename view_type<sample_t, layout_t, planar, step, mut>::type type;
};


/**
   \brief Constructs a homogeneous buffer type from a source buffer
   type by changing some of the properties.
   
   \ingroup TypeFactoryDerived
   Use boost::use_default for the properties of the source buffer that you
   want to keep
*/
template <typename Buffer, typename T=boost::use_default,
	  typename L=boost::use_default,
	  typename IsPlanar=boost::use_default>
class derived_buffer_type
{
    typedef typename  boost::mpl::if_<
	boost::is_same<T ,boost::use_default>,
	typename sample_type<Buffer>::type,
	T >::type sample_t;

    typedef typename  boost::mpl::if_<
	boost::is_same<L,boost::use_default>, 
	layout<typename channel_space_type<Buffer>::type,
	       typename sample_mapping_type<Buffer>::type>,
	L>::type layout_t;

    static const bool planar = boost::mpl::if_<
	boost::is_same<IsPlanar,boost::use_default>,
	is_planar<Buffer>,
	IsPlanar>::type::value;

public:
    typedef typename buffer_type<sample_t, layout_t, planar>::type type;
};

} /* namespace psynth::sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_METAFUNCTIONS */
