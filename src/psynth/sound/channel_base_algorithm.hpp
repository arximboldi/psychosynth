/**
 *  Time-stamp:  <2010-10-29 15:14:04 raskolnikov>
 *
 *  @file        channel_base_algorithm.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Oct 29 13:53:18 2010
 *
 *  Frame related algorithms.
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


#ifndef PSYNTH_SOUND_CHANNEL_BASE_ALGORITHM_HPP
#define PSYNTH_SOUND_CHANNEL_BASE_ALGORITHM_HPP

#include <algorithm>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/at.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/util.hpp>

namespace psynth
{
namespace sound
{

/*
 *
 *   size:   Semantic sample size
 *
 */

/**
   \defgroup ChannelBaseAlgorithmSize size
   \ingroup ChannelBaseAlgorithm
   \brief Returns an MPL integral type specifying the number of
   elements in a channel base

   Example:
   \code
   BOOST_STATIC_ASSERT((size<rgb8_frame_t>::value == 3));
   BOOST_STATIC_ASSERT((size<cmyk8_planar_ptr_t>::value == 4));
   \endcode
*/

/**
   \brief Returns an MPL integral type specifying the number of
   elements in a channel base
   
   \ingroup ChannelBaseAlgorithmSize
*/
template <typename ChannelBase>
struct size : public mpl::size<typename ChannelBase::layout_t::channel_space_t> {};


/*
 *
 *    semantic_at_c:   Semantic sample accessors
 *
 */

/**
   \defgroup ChannelBaseAlgorithmSemanticAtC
   kth_semantic_element_type, kth_semantic_element_reference_type,
   kth_semantic_element_const_reference_type, semantic_at_c
   
   \ingroup ChannelBaseAlgorithm
   \brief Support for accessing the elements of a channel base by
   semantic index
   
   The semantic index of an element is the index of its channel in the
   channel space. Semantic indexing allows for proper pairing of
   elements of channel bases independent on their layout. For example,
   red is the first semantic element of a channel base regardless of
   whether it has an RGB layout or a BGR layout.  All GIL channel base
   algorithms taking multiple channel bases use semantic indexing to
   access their elements.

   Example:
   \code
    // 16-bit BGR frame, 4 bits for the blue, 3 bits for the green, 2
    // bits for the red sample and 7 unused bits

    typedef packed_frame_type<uint16_t,
    mpl::vector3_c<unsigned,4,3,2>, bgr_layout_t>::type
    bgr432_frame_t;
    

    // A reference to its red sample. Although the red sample is the
    // third, its semantic index is 0 in the RGB channel space
    typedef kth_semantic_element_reference_type<bgr432_frame_t,
    0>::type red_sample_reference_t;

    // Initialize the frame to black
    bgr432_frame_t red_frame(0,0,0);

    // Set the red sample to 100%
    red_sample_reference_t red_sample = semantic_at_c<0>(red_frame);
    red_sample = sample_traits<red_sample_reference_t>::max_value();       
    \endcode
*/

/**
   \brief Specifies the type of the K-th semantic element of a channel
   base
   \ingroup ChannelBaseAlgorithmSemanticAtC
*/
template <typename ChannelBase, int K>
struct kth_semantic_element_type
{
    BOOST_STATIC_CONSTANT(
	int, semantic_index = (
	    mpl::at_c<typename ChannelBase::layout_t::sample_mapping_t,
		      K>::type::value));
    typedef typename kth_element_type <ChannelBase, semantic_index>::type type;
};

/**
   \brief Specifies the return type of the mutable
   semantic_at_c<K>(channel_base);
   \ingroup ChannelBaseAlgorithmSemanticAtC
*/
template <typename ChannelBase, int K>
struct kth_semantic_element_reference_type
{
    BOOST_STATIC_CONSTANT(
	int, semantic_index = (
	    mpl::at_c<typename ChannelBase::layout_t::sample_mapping_t,
		      K>::type::value));
    typedef typename kth_element_reference_type<
	ChannelBase, semantic_index>::type type;

    static type get (ChannelBase& cb)
    {
	return gil::at_c<semantic_index>(cb);
    }
};


/**
  \brief Specifies the return type of the constant
  semantic_at_c<K>(channel_base);
  
  \ingroup ChannelBaseAlgorithmSemanticAtC
*/
template <typename ChannelBase, int K>
struct kth_semantic_element_const_reference_type
{
    BOOST_STATIC_CONSTANT(
	int, semantic_index = (
	    mpl::at_c<typename ChannelBase::layout_t::sample_mapping_t,
		      K>::type::value));
    typedef typename kth_element_const_reference_type<
	ChannelBase, semantic_index>::type type;

    static type get (const ChannelBase& cb)
    {
	return gil::at_c<semantic_index> (cb);
    }
};

/**
   \brief A mutable accessor to the K-th semantic element of a channel
   base
   \ingroup ChannelBaseAlgorithmSemanticAtC
*/
template <int K, typename ChannelBase> inline
typename disable_if<
    is_const<ChannelBase>,
    typename kth_semantic_element_reference_type<ChannelBase,K>::type>::type
semantic_at_c (ChannelBase& p)
{ 
    return kth_semantic_element_reference_type<ChannelBase, K>::get(p);
}

/**
   \brief A constant accessor to the K-th semantic element of a
   channel base
   \ingroup ChannelBaseAlgorithmSemanticAtC
*/
template <int K, typename ChannelBase> inline
typename kth_semantic_element_const_reference_type<ChannelBase,K>::type
semantic_at_c (const ChannelBase& p)
{ 
    return kth_semantic_element_const_reference_type<ChannelBase,K>::get (p); 
}


/*
 *
 *   get_channel:   Named sample accessors
 *
 */

/**
   \defgroup ChannelBaseAlgorithmChannel channel_element_type,
   channel_element_reference_type,
   channel_element_const_reference_type, get_channel, contains_channel
   
   \ingroup ChannelBaseAlgorithm
   \brief Support for accessing the elements of a channel base by
   channel name

   Example: A function that takes a generic frame containing a red
   sample and sets it to 100%:

   \code
   template <typename Frame>
   void set_red_to_max(Frame& frame) {
       boost::function_requires<MutableFrameConcept<Frame> >();
       BOOST_STATIC_ASSERT((contains_channel<Frame, red_t>::value));

       typedef typename channel_element_type<Frame, red_t>::type red_sample_t;
       get_channel(frame, red_t()) = sample_traits<red_sample_t>::max_value(); 
   }
   \endcode
*/

/**
   \brief A predicate metafunction determining whether a given channel
   base contains a given channel
   
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel>
struct contains_channel :
    public mpl::contains<typename ChannelBase::layout::channel_space,
			 Channel> {};

template <typename ChannelBase, typename Channel>
struct channel_index_type :
    public detail::type_to_index<typename ChannelBase::layout::channel_space,
				 Channel> {};

/**
   \brief Specifies the type of the element associated with a given
   channel tag
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel>
struct channel_element_type : public kth_semantic_element_type<
    ChannelBase, channel_index_type<ChannelBase,Channel>::value> {};

/**
   \brief Specifies the return type of the mutable element accessor by
   channel name, get_channel(channel_base, Channel());
   
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel>
struct channel_element_reference_type :
    public kth_semantic_element_reference_type<
    ChannelBase, channel_index_type<ChannelBase, Channel>::value> {};

/**
   \brief Specifies the return type of the constant element accessor
   by channel name, get_channel(channel_base, Channel());
   
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel>
struct channel_element_const_reference_type :
    public kth_semantic_element_const_reference_type<
    ChannelBase, channel_index_type<ChannelBase, Channel>::value> {};

/**
   \brief Mutable accessor to the element associated with a given
   channel name
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel> 
typename channel_element_reference_type<ChannelBase,Channel>::type
get_channel (ChannelBase& cb, Channel = Channel())
{
    return channel_element_reference_type<ChannelBase, Channel>::get (cb);
}

/**
   \brief Constant accessor to the element associated with a given channel name
   \ingroup ChannelBaseAlgorithmChannel
*/
template <typename ChannelBase, typename Channel> 
typename channel_element_const_reference_type<ChannelBase,Channel>::type
get_channel (const ChannelBase& cb, Channel=Channel())
{
    return channel_element_const_reference_type<ChannelBase,Channel>::get (cb);
}


/*
 *
 *    element_type, element_reference_type,
 *    element_const_reference_type: Support for homogeneous channel
 *    bases
 *
 */

/**
   \defgroup ChannelBaseAlgorithmHomogeneous element_type,
   element_reference_type, element_const_reference_type
   
   \ingroup ChannelBaseAlgorithm
   \brief Types for homogeneous channel bases

   Example:
   \code
   typedef element_type<rgb8c_planar_ptr_t>::type element_t;
   BOOST_STATIC_ASSERT((boost::is_same<element_t, const bits8*>::value));
   \endcode
*/

/**
  \brief Specifies the element type of a homogeneous channel base
  \ingroup ChannelBaseAlgorithmHomogeneous
*/
template <typename ChannelBase>
struct element_type : public kth_element_type<ChannelBase, 0> {};

/**
   \brief Specifies the return type of the mutable element accessor
   at_c of a homogeneous channel base
   
   \ingroup ChannelBaseAlgorithmHomogeneous
*/
template <typename ChannelBase>
struct element_reference_type :
    public kth_element_reference_type<ChannelBase, 0> {};

/**
   \brief Specifies the return type of the constant element accessor
   at_c of a homogeneous channel base
   
   \ingroup ChannelBaseAlgorithmHomogeneous
*/
template <typename ChannelBase>
struct element_const_reference_type :
    public kth_element_const_reference_type<ChannelBase, 0> {};


namespace detail
{

/* compile-time recursion for per-element operations on channel bases */
template <int N>
struct element_recursion
{
    // static_equal
    template <typename P1,typename P2>
    static bool static_equal (const P1& p1, const P2& p2)
    { 
        return element_recursion<N-1>::static_equal(p1,p2) &&
               semantic_at_c<N-1>(p1) == semantic_at_c<N-1>(p2); 
    }
    
    //static_copy
    template <typename P1,typename P2>
    static void static_copy(const P1& p1, P2& p2)
    {
        element_recursion<N-1>::static_copy(p1,p2);
        semantic_at_c<N-1>(p2) = semantic_at_c<N-1>(p1);
    }
    
    //static_fill
    template <typename P,typename T2>
    static void static_fill(P& p, T2 v)
    {
        element_recursion<N-1>::static_fill (p,v);
        semantic_at_c<N-1>(p) = v;
    }
    
    //static_generate
    template <typename Dst,typename Op> 
    static void static_generate (Dst& dst, Op op)
    {
        element_recursion<N-1>::static_generate (dst, op);
        semantic_at_c<N-1>(dst) = op();
    }
    
    //static_for_each with one source
    template <typename P1,typename Op> 
    static Op static_for_each (P1& p1, Op op)
    {
        Op op2 (element_recursion<N-1>::static_for_each (p1,op));
        op2 (semantic_at_c<N-1> (p1));
        return op2;
    }
    
    template <typename P1,typename Op> 
    static Op static_for_each (const P1& p1, Op op)
    {
        Op op2 (element_recursion<N-1>::static_for_each (p1,op));
        op2 (semantic_at_c<N-1> (p1));
        return op2;
    }
    
    //static_for_each with two sources
    template <typename P1,typename P2,typename Op> 
    static Op static_for_each (P1& p1, P2& p2, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each (p1, p2, op));
        op2(semantic_at_c<N-1>(p1), semantic_at_c<N-1>(p2));
        return op2;
    }
    
    template <typename P1,typename P2,typename Op> 
    static Op static_for_each(P1& p1, const P2& p2, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,op));
        op2(semantic_at_c<N-1>(p1), semantic_at_c<N-1>(p2));
        return op2;
    }

    template <typename P1,typename P2,typename Op> 
    static Op static_for_each(const P1& p1, P2& p2, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,op));
        op2(semantic_at_c<N-1>(p1), semantic_at_c<N-1>(p2));
        return op2;
    }

    template <typename P1,typename P2,typename Op> 
    static Op static_for_each(const P1& p1, const P2& p2, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,op));
        op2(semantic_at_c<N-1>(p1), semantic_at_c<N-1>(p2));
        return op2;
    }

    //static_for_each with three sources
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (P1& p1, P2& p2, P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2(semantic_at_c<N-1>(p1),
	    semantic_at_c<N-1>(p2),
	    semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (P1& p1, P2& p2, const P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (P1& p1, const P2& p2, P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (P1& p1, const P2& p2, const P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (const P1& p1, P2& p2, P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (const P1& p1, P2& p2, const P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (const P1& p1, const P2& p2, P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each (const P1& p1, const P2& p2, const P3& p3, Op op)
    {
        Op op2(element_recursion<N-1>::static_for_each(p1,p2,p3,op));
        op2 (semantic_at_c<N-1>(p1),
	     semantic_at_c<N-1>(p2),
	     semantic_at_c<N-1>(p3));
        return op2;
    }
    
    //static_transform with one source
    template <typename P1,typename Dst,typename Op> 
    static Op static_transform (P1& src, Dst& dst, Op op)
    {
        Op op2(element_recursion<N-1>::static_transform(src,dst,op));
        semantic_at_c<N-1>(dst)=op2(semantic_at_c<N-1>(src));
        return op2;
    }
    
    template <typename P1,typename Dst,typename Op> 
    static Op static_transform (const P1& src, Dst& dst, Op op)
    {
        Op op2(element_recursion<N-1>::static_transform(src,dst,op));
        semantic_at_c<N-1>(dst)=op2(semantic_at_c<N-1>(src));
        return op2;
    }
    
    //static_transform with two sources
    template <typename P1,typename P2,typename Dst,typename Op>
    static Op static_transform (P1& src1, P2& src2, Dst& dst, Op op)
    {
        Op op2(element_recursion<N-1>::static_transform(src1,src2,dst,op));
        semantic_at_c<N-1>(dst) = op2 (semantic_at_c<N-1>(src1),
				       semantic_at_c<N-1>(src2));
        return op2;
    }
    
    template <typename P1,typename P2,typename Dst,typename Op>
    static Op static_transform (P1& src1, const P2& src2, Dst& dst, Op op) {
        Op op2(element_recursion<N-1>::static_transform(src1,src2,dst,op));
        semantic_at_c<N-1>(dst) = op2 (semantic_at_c<N-1>(src1),
				       semantic_at_c<N-1>(src2));
        return op2;
    }
    
    template <typename P1,typename P2,typename Dst,typename Op>
    static Op static_transform (const P1& src1, P2& src2, Dst& dst, Op op)
    {
        Op op2(element_recursion<N-1>::static_transform(src1,src2,dst,op));
        semantic_at_c<N-1>(dst) = op2 (semantic_at_c<N-1>(src1),
				       semantic_at_c<N-1>(src2));
        return op2;
    }
    
    template <typename P1,typename P2,typename Dst,typename Op>
    static Op static_transform (const P1& src1, const P2& src2, Dst& dst, Op op)
    {
        Op op2(element_recursion<N-1>::static_transform(src1,src2,dst,op));
        semantic_at_c<N-1>(dst) = op2 (semantic_at_c<N-1>(src1),
				       semantic_at_c<N-1>(src2));
        return op2;
    }
};

// Termination condition of the compile-time recursion for element
// operations on a channel base
template<> struct element_recursion<0>
{
    //static_equal
    template <typename P1,typename P2>
    static bool static_equal(const P1&, const P2&) { return true; }
    //static_copy
    template <typename P1,typename P2>
    static void static_copy(const P1&, const P2&) {}
    //static_fill
    template <typename P, typename T2>
    static void static_fill(const P&, T2) {}
    //static_generate
    template <typename Dst,typename Op>
    static void static_generate(const Dst&,Op) {}
    //static_for_each with one source
    template <typename P1,typename Op>
    static Op static_for_each(const P1&,Op op) { return op; }
    //static_for_each with two sources
    template <typename P1,typename P2,typename Op>
    static Op static_for_each(const P1&,const P2&,Op op) { return op; }
    //static_for_each with three sources
    template <typename P1,typename P2,typename P3,typename Op>
    static Op static_for_each(const P1&,const P2&,const P3&,Op op) { return op; }
    //static_transform with one source
    template <typename P1,typename Dst,typename Op>
    static Op static_transform(const P1&,const Dst&,Op op) { return op; }
    //static_transform with two sources
    template <typename P1,typename P2,typename Dst,typename Op>
    static Op static_transform(const P1&,const P2&,const Dst&,Op op) { return op; }
};

// std::min and std::max don't have the mutable overloads...
template <typename Q> inline const Q& mutable_min(const Q& x, const Q& y)
{ return x<y ? x : y; }
template <typename Q> inline       Q& mutable_min(      Q& x,       Q& y)
{ return x<y ? x : y; }
template <typename Q> inline const Q& mutable_max(const Q& x, const Q& y)
{ return x<y ? y : x; }
template <typename Q> inline       Q& mutable_max(      Q& x,       Q& y)
{ return x<y ? y : x; }

// compile-time recursion for min/max element
template <int N>
struct min_max_recur
{
    template <typename P>
    static typename element_const_reference_type<P>::type max_ (const P& p)
    {
        return mutable_max(min_max_recur<N-1>::max_(p), semantic_at_c<N-1>(p));
    }
    
    template <typename P>
    static typename element_reference_type<P>::type       max_ (P& p)
    {
        return mutable_max(min_max_recur<N-1>::max_(p), semantic_at_c<N-1>(p));
    }
    
    template <typename P>
    static typename element_const_reference_type<P>::type min_ (const P& p)
    {
        return mutable_min(min_max_recur<N-1>::min_(p), semantic_at_c<N-1>(p));
    }
    
    template <typename P>
    static typename element_reference_type<P>::type       min_ (P& p)
    {
        return mutable_min(min_max_recur<N-1>::min_(p), semantic_at_c<N-1>(p));
    }    
};

// termination condition of the compile-time recursion for min/max
// element
template <>
struct min_max_recur<1>
{
    template <typename P>
    static typename element_const_reference_type<P>::type max_(const P& p)
    { return semantic_at_c<0>(p); }
    template <typename P>
    static typename element_reference_type<P>::type       max_(P& p)
    { return semantic_at_c<0>(p); }
    template <typename P>
    static typename element_const_reference_type<P>::type min_(const P& p)
    { return semantic_at_c<0>(p); }
    template <typename P>
    static typename element_reference_type<P>::type       min_(P& p)
    { return semantic_at_c<0>(p); }
};

} /* namespace detail */


/**
   \defgroup ChannelBaseAlgorithmMinMax static_min, static_max
   \ingroup ChannelBaseAlgorithm
   \brief Equivalents to std::min_element and std::max_element for
   homogeneous channel bases

   Example:
   \code
   rgb8_frame_t frame(10,20,30);
   assert(frame[2] == 30);
   static_max(frame) = static_min(frame);
   assert(frame[2] == 10);
   \endcode
   \{
*/

template <typename P>
PSYNTH_FORCEINLINE
typename element_const_reference_type<P>::type static_max (const P& p)
{ return detail::min_max_recur<size<P>::value>::max_(p); }

template <typename P>
PSYNTH_FORCEINLINE
typename element_reference_type<P>::type       static_max (P& p)
{ return detail::min_max_recur<size<P>::value>::max_(p); }

template <typename P>
PSYNTH_FORCEINLINE
typename element_const_reference_type<P>::type static_min (const P& p)
{ return detail::min_max_recur<size<P>::value>::min_(p); }

template <typename P>
PSYNTH_FORCEINLINE
typename element_reference_type<P>::type       static_min (P& p)
{ return detail::min_max_recur<size<P>::value>::min_(p); }

/// \}

/**
   \defgroup ChannelBaseAlgorithmEqual static_equal 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::equal. Pairs the elements semantically

   Example:
   \code
   rgb8_frame_t rgb_red(255,0,0);
   bgr8_frame_t bgr_red(0,0,255);
   assert(rgb_red[0]==255 && bgr_red[0]==0);
   
   assert(static_equal(rgb_red,bgr_red));
   assert(rgb_red==bgr_red);  // operator== invokes static_equal
   \endcode
   \{
*/

template <typename P1,typename P2>
PSYNTH_FORCEINLINE
bool static_equal (const P1& p1, const P2& p2)
{ return detail::element_recursion<size<P1>::value>::static_equal(p1,p2); }

/// \}

/**
   \defgroup ChannelBaseAlgorithmCopy static_copy 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::copy. Pairs the elements semantically
   
   Example:
   \code
   rgb8_frame_t rgb_red(255,0,0);
   bgr8_frame_t bgr_red;
   static_copy(rgb_red, bgr_red);  // same as bgr_red = rgb_red
   
   assert(rgb_red[0] == 255 && bgr_red[0] == 0);
   assert(rgb_red == bgr_red);
   \endcode
   \{
*/

template <typename Src,typename Dst>
PSYNTH_FORCEINLINE
void static_copy(const Src& src, Dst& dst)
{  detail::element_recursion<size<Dst>::value>::static_copy(src,dst); }

/// \}

/**
   \defgroup ChannelBaseAlgorithmFill static_fill 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::fill.
   
   Example:
   \code
   rgb8_frame_t p;
   static_fill(p, 10);
   assert(p == rgb8_frame_t(10,10,10));
   \endcode
   \{
*/
template <typename P,typename V>
PSYNTH_FORCEINLINE
void static_fill (P& p, const V& v)
{  detail::element_recursion<size<P>::value>::static_fill(p,v); }
/// \}

/**
   \defgroup ChannelBaseAlgorithmGenerate static_generate 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::generate.
   
   Example: Set each sample of a frame to its semantic index. The
   samples must be assignable from an integer.
   
   \code
   struct consecutive_fn {
     int& _current;
     consecutive_fn(int& start) : _current(start) {}
     int operator()() { return _current++; }
   };
   rgb8_frame_t p;
   int start=0;
   static_generate(p, consecutive_fn(start));
   assert(p == rgb8_frame_t(0,1,2));
\endcode
\{
*/

template <typename P1,typename Op>
PSYNTH_FORCEINLINE void static_generate(P1& dst,Op op)
{ detail::element_recursion<size<P1>::value>::static_generate(dst,op); }

/** \} */

/**
   \defgroup ChannelBaseAlgorithmTransform static_transform 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::transform. Pairs the elements
   semanticall

   Example: Write a generic function that adds two frames into a
   homogeneous result frame.
   
   \code
   template <typename Result>
   struct my_plus {
      template <typename T1, typename T2>
      Result operator()(T1 f1, T2 f2) const { return f1+f2; }
   };

   template <typename Frame1, typename Frame2, typename Frame3>
   void sum_samples(const Frame1& p1, const Frame2& p2, Frame3& result) {
       typedef typename sample_type<Frame3>::type result_sample_t;
       static_transform(p1,p2,result,my_plus<result_sample_t>());
   }

   rgb8_frame_t p1(1,2,3);
   bgr8_frame_t p2(3,2,1);
   rgb8_frame_t result;
   sum_samples(p1,p2,result);
   assert(result == rgb8_frame_t(2,4,6));
   \endcode
   \{
*/

//static_transform with one source
template <typename Src,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform(Src& src,Dst& dst,Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform(
	src, dst, op);
}

template <typename Src,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform(const Src& src,Dst& dst,Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform (
	src, dst, op);
}

//static_transform with two sources
template <typename P2,typename P3,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform (P2& p2, P3& p3, Dst& dst, Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform (
	p2, p3, dst, op);
}

template <typename P2,typename P3,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform(P2& p2,const P3& p3,Dst& dst,Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform (
	p2, p3, dst, op);
}

template <typename P2,typename P3,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform (const P2& p2, P3& p3, Dst& dst, Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform (
	p2, p3, dst, op);
}

template <typename P2,typename P3,typename Dst,typename Op>
PSYNTH_FORCEINLINE
Op static_transform (const P2& p2, const P3& p3, Dst& dst, Op op)
{
    return detail::element_recursion<size<Dst>::value>::static_transform (
	p2, p3, dst, op);
}

/** \} */

/**
   \defgroup ChannelBaseAlgorithmForEach static_for_each 
   \ingroup ChannelBaseAlgorithm
   \brief Equivalent to std::for_each. Pairs the elements semantically
   
   Example: Use static_for_each to increment a planar frame iterator
   \code
   struct increment { 
      template <typename Incrementable> 
      void operator()(Incrementable& x) const { ++x; } 
   };

   template <typename ChannelBase>
   void increment_elements(ChannelBase& cb) {
       static_for_each(cb, increment());
   }

   bits8 red[2], green[2], blue[2];
   rgb8c_planar_ptr_t p1(red,green,blue);
   rgb8c_planar_ptr_t p2=p1;
   increment_elements(p1);
   ++p2;
   assert(p1 == p2);
   \endcode
   \{
*/

//static_for_each with one source
template <typename P1,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each (P1& p1, Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each (p1,op); }

template <typename P1,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each (const P1& p1, Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each(p1,op); }

//static_for_each with two sources
template <typename P1,typename P2,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each (P1& p1, P2& p2, Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each (
	p1, p2, op);
}

template <typename P1,typename P2,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(P1& p1,const P2& p2, Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each(p1,p2,op); }

template <typename P1,typename P2,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1, P2& p2, Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each(p1,p2,op); }

template <typename P1,typename P2,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1,const P2& p2, Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each(p1,p2,op); }

//static_for_each with three sources
template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(P1& p1,P2& p2,P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each(
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(P1& p1,P2& p2,const P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each(
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(P1& p1,const P2& p2,P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each(
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(P1& p1,const P2& p2,const P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each (
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1,P2& p2,P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each (
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1,P2& p2,const P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each (
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1,const P2& p2,P3& p3,Op op)
{
    return detail::element_recursion<size<P1>::value>::static_for_each(
	p1,p2,p3,op);
}

template <typename P1,typename P2,typename P3,typename Op>
PSYNTH_FORCEINLINE
Op static_for_each(const P1& p1,const P2& p2,const P3& p3,Op op)
{ return detail::element_recursion<size<P1>::value>::static_for_each (
	p1,p2,p3,op);
}

///\}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_CHANNEL_BASE_ALGORITHM */
