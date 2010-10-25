/**
 *  Time-stamp:  <2010-10-25 19:13:31 raskolnikov>
 *
 *  @file        concept.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Oct 20 20:01:23 2010
 *
 *  Concepts of the sound library.
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


#ifndef GIL_CONCEPT_H
#define GIL_CONCEPT_H

#include <psynth/base/concepts.hpp>

#include <functional>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/concept_check.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/size.hpp>

namespace psynth
{
namespace sound
{

template <typename T> struct sample_traits;
template <typename P> struct is_frame;
template <typename dstT, typename srcT>

typename sample_traits<dstT>::value_type sample_convert (const srcT& val);
template <typename T> class point2;
template <std::size_t K, typename T>
const T& axis_value(const point2<T>& p);
template <std::size_t K, typename T>
T& axis_value(point2<T>& p);

template <typename ChannelBase, int K> struct
kth_element_type;
template <typename ChannelBase, int K> struct
kth_element_reference_type;
template <typename ChannelBase, int K> struct
kth_element_const_reference_type;
template <typename ChannelBase, int K>
struct kth_semantic_element_reference_type;
template <typename ChannelBase, int K>
struct kth_semantic_element_const_reference_type;

template <typename ChannelBase> struct size;
template <typename ChannelBase> struct element_type;
template <typename T> struct sample_type;
template <typename T> struct channel_space_type;
template <typename T> struct sample_mapping_type;
template <typename T> struct is_planar;
template <typename T> struct num_samples;

template <typename It> struct const_iterator_type;
template <typename It> struct iterator_is_mutable;
template <typename It> struct is_iterator_adaptor;
template <typename It, typename NewBaseIt> struct iterator_adaptor_rebind;
template <typename It> struct iterator_adaptor_get_base;

/* forward-declare at_c */
namespace detail
{
template <typename Element, typename Layout, int K>
struct homogeneous_channel_base;
} /* namespace detail */

template <int K, typename E, typename L, int N>
typename add_reference<E>::type at_c (
    detail::homogeneous_channel_base<E,L,N>& p);

template <int K, typename E, typename L, int N>
typename add_reference<typename add_const<E>::type>::type at_c (
    const detail::homogeneous_channel_base<E,L,N>& p);

#if !defined(_MSC_VER)  || _MSC_VER > 1310
template <typename P, typename C, typename L> struct packed_frame;
template <int K, typename P, typename C, typename L>
typename kth_element_reference_type<packed_frame<P,C,L>, K>::type 
at_c (packed_frame<P,C,L>& p);

template <int K, typename P, typename C, typename L>
typename kth_element_const_reference_type<packed_frame<P,C,L>,K>::type 
at_c (const packed_frame<P,C,L>& p);

template <typename B, typename C, typename L, bool M>
struct bit_aligned_frame_reference;

template <int K, typename B, typename C, typename L, bool M> inline
typename kth_element_reference_type<
    bit_aligned_frame_reference<B,C,L,M>, K>::type
at_c(const bit_aligned_frame_reference<B,C,L,M>& p);
#endif

/* Forward-declare semantic_at_c */
template <int K, typename ChannelBase>
typename disable_if<
    is_const<ChannelBase>,
    typename kth_semantic_element_reference_type<ChannelBase,K>::type>::type
semantic_at_c (ChannelBase& p);

template <int K, typename ChannelBase>
typename kth_semantic_element_const_reference_type<ChannelBase,K>::type
semantic_at_c(const ChannelBase& p);

template <typename T> struct dynamic_x_step_type;
template <typename T> struct dynamic_y_step_type;
template <typename T> struct transposed_type;

namespace detail
{
template <typename T>
void initialize_it (T& x) {}
} /* namespace detail */

template <typename T>
struct remove_const_and_reference :
	public remove_const<typename remove_reference<T>::type> {};

/*
 *
 *  Point concepts.
 *
 */

/**
   \brief N-dimensional point concept
   \ingroup PointConcept
*/
/**
\code
concept PointNDConcept<typename T> : Regular<T> {    
    // the type of a coordinate along each axis
    template <size_t K> struct axis; where Metafunction<axis>;
            
    const size_t num_dimensions;
    
    // accessor/modifier of the value of each axis.
    template <size_t K> const typename axis<K>::type& T::axis_value() const;
    template <size_t K>       typename axis<K>::type& T::axis_value();
};
\endcode
*/

template <typename P>
struct PointNDConcept
{
    void constraints() {
        gil_function_requires< Regular<P> >();

        typedef typename P::value_type value_type;
        static const std::size_t N = P::num_dimensions;
	ignore_unused_variable_warning (N);
        typedef typename P::template axis<0>::coord_t FT;
        typedef typename P::template axis<N-1>::coord_t LT;
        FT ft=gil::axis_value<0>(point);
        axis_value<0>(point)=ft;
        LT lt=axis_value<N-1>(point);
        axis_value<N-1> (point) = lt;
    
        value_type v = point[0];
	ignore_unused_variable_warning (v);
        point[0] = point[0];
    }
    P point;
};

/**
   \brief 2-dimensional point concept
   \ingroup PointConcept
*/
/**
\code
concept Point2DConcept<typename T> : PointNDConcept<T> {    
    where num_dimensions == 2;
    where SameType<axis<0>::type, axis<1>::type>;

    typename value_type = axis<0>::type;

    const value_type& operator[](const T&, size_t i);
          value_type& operator[](      T&, size_t i);

    value_type x,y;
};
\endcode
*/

template <typename P>
struct Point2DConcept
{
    void constraints() {
        gil_function_requires< PointNDConcept<P> >();
        BOOST_STATIC_ASSERT(P::num_dimensions == 2);
        point.x=point.y;
        point[0]=point[1];
    }
    P point;
};


namespace detail
{

template <class TT>
    /* Preconditions: TT Models
       boost_concepts::ForwardTraversalConcept */
struct ForwardIteratorIsMutableConcept
{
    void constraints() {
	*i++ = *i;    // require postincrement and assignment
    }
    TT i;
};

template <class TT>
/* Preconditions: TT Models boost::BidirectionalIteratorConcept */
struct BidirectionalIteratorIsMutableConcept
{
    void constraints() {
	gil_function_requires< ForwardIteratorIsMutableConcept<TT> >();
	*i-- = *i;                  // require postdecrement and assignment
    }
    TT i;
};

template <class TT>
/* Preconditions: TT Models
   boost_concepts::RandomAccessTraversalConcept */
struct RandomAccessIteratorIsMutableConcept
{
    void constraints() {
	gil_function_requires< BidirectionalIteratorIsMutableConcept<TT> >();
	typename std::iterator_traits<TT>::difference_type n=0;
	ignore_unused_variable_warning(n);
	i[n] = *i;
	// require element access and assignment
    }
    TT i;
};

} /* namespace detail */


/*
 *
 *  Channel Space concepts.
 *
 */

/**
   \brief Channel space type concept
   \ingroup ChannelSpaceAndLayoutConcept
*/

/**
\code
concept ChannelSpaceConcept<MPLRandomAccessSequence Cs> {
   // An MPL Random Access Sequence, whose elements are channel tags
};
\endcode
*/
template <typename Cs>
struct ChannelSpaceConcept
{
    void constraints() {
        // An MPL Random Access Sequence, whose elements are channel tags
    }
};

template <typename ChannelSpace1, typename ChannelSpace2>  // Models ChannelSpaceConcept
struct channel_spaces_are_compatible : public is_same<ChannelSpace1,ChannelSpace2> {};

/**
   \brief Two channel spaces are compatible if they are the same
   \ingroup ChannelSpaceAndLayoutConcept
*/
/**
\code
concept ChannelSpacesCompatibleConcept<ChannelSpaceConcept Cs1,
    ChannelSpaceConcept Cs2> {
    where SameType<Cs1,Cs2>;
};
\endcode
*/
template <typename Cs1, typename Cs2>
struct ChannelSpacesCompatibleConcept
{
    void constraints() {
        BOOST_STATIC_ASSERT((channel_spaces_are_compatible<Cs1,Cs2>::value));
    }
};


/*
 *
 *  Sample concepts
 *
 */

/**
   \brief Sample mapping concept
   \ingroup ChannelSpaceAndLayoutConcept
*/
/**
\code
concept SampleMappingConcept<MPLRandomAccessSequence CM> {
   // An MPL Random Access Sequence, whose elements model
   // MPLIntegralConstant representing a permutation
};
\endcode
*/
template <typename CM>
struct SampleMappingConcept
{
    void constraints() {
        // An MPL Random Access Sequence, whose elements model
        // MPLIntegralConstant representing a permutation
    }
};


/**
   \ingroup SampleConcept

   \brief A sample is the building block of a channel. Channel is defined
   as a mixture of primary channels and a sample defines the degree to
   which each primary channel is used in the mixture.
*/

/**         
   For example, in the RGB channel space, using 8-bit unsigned samples,
   the channel red is defined as [255 0 0], which means maximum of Red, and
   no Green and Blue.
    
   Built-in scalar types, such as \p int and \p float, are valid GIL
   samples. In more complex scenarios, samples may be represented as
   bit ranges or even individual bits.  In such cases special classes are
   needed to represent the value and reference to a sample.
    
   Samples have a traits class, \p sample_traits, which defines
   their associated types as well as their operating ranges.

\code
concept SampleConcept<typename T> : EqualityComparable<T> {
    typename value_type      = T;
    // use sample_traits<T>::value_type to access it
    typename reference       = T&;
    // use sample_traits<T>::reference to access it
    typename pointer         = T*;
    // use sample_traits<T>::pointer to access it
    typename const_reference = const T&;
    // use sample_traits<T>::const_reference to access it
    typename const_pointer   = const T*;
    // use sample_traits<T>::const_pointer to access it
    static const bool is_mutable;
    // use sample_traits<T>::is_mutable to access it

    static T min_value(); // use sample_traits<T>::min_value to access it
    static T max_value(); // use sample_traits<T>::min_value to access it
};
\endcode
*/
template <typename T>
struct SampleConcept
{
    void constraints() {
        gil_function_requires< boost::EqualityComparableConcept<T> >(); 
        
        typedef typename sample_traits<T>::value_type v;
        typedef typename sample_traits<T>::reference r;
        typedef typename sample_traits<T>::pointer p;
        typedef typename sample_traits<T>::const_reference cr;
        typedef typename sample_traits<T>::const_pointer cp;

        sample_traits<T>::min_value();
        sample_traits<T>::max_value();
    }

     T c;
};

namespace detail
{
    // Preconditions: T models SampleConcept
    template <typename T>
    struct SampleIsMutableConcept
    {
        void constraints() {
            c=c;
            using std::swap;
            swap(c,c);
        }
        T c;
    };
} /* namespace detail*/

/**
   \brief A sample that allows for modifying its value
   \ingroup SampleConcept
*/
/**
\code
concept MutableSampleConcept<SampleConcept T> : Assignable<T>, Swappable<T> {};
\endcode
*/
template <typename T>
struct MutableSampleConcept
{
    void constraints() {
        gil_function_requires<SampleConcept<T> >();
        gil_function_requires<detail::SampleIsMutableConcept<T> >();
    }
};

/**
   \brief A sample that supports default construction. 
   \ingroup SampleConcept
*/
/**
\code
concept SampleValueConcept<SampleConcept T> : Regular<T> {}; 
\endcode
*/
template <typename T>
struct SampleValueConcept
{
    void constraints() {
        gil_function_requires<SampleConcept<T> >();
        gil_function_requires<Regular<T> >();
    }
};

/**
   \brief Predicate metafunction returning whether two samples are
   compatible
   \ingroup SampleAlgorithm

   Samples are considered compatible if their value types (ignoring
   constness and references) are the same.
*/
/**
Example:

\code
BOOST_STATIC_ASSERT((samples_are_compatible<bits8, const bits8&>::value));
\endcode
*/
template <typename T1, typename T2>  // Models GIL Frame
struct samples_are_compatible 
    : public is_same<typename sample_traits<T1>::value_type,
		     typename sample_traits<T2>::value_type> {};

/**
   \brief Samples are compatible if their associated value types
   (ignoring constness and references) are the same
   \ingroup SampleConcept
*/
/**
\code
concept SamplesCompatibleConcept<SampleConcept T1, SampleConcept T2> {
    where SameType<T1::value_type, T2::value_type>;
};
\endcode
*/
template <typename T1, typename T2>
struct SamplesCompatibleConcept
{
    void constraints() {
        BOOST_STATIC_ASSERT((samples_are_compatible<T1,T2>::value));
    }
};

/**
   \brief A sample is convertible to another one if the \p
   sample_convert algorithm is defined for the two samples

   Convertibility is non-symmetric and implies that one sample can be
   converted to another. Conversion is explicit and often lossy
   operation.
   \ingroup SampleConcept
*/
/**
\code
concept SampleConvertibleConcept<SampleConcept SrcSample,
     SampleValueConcept DstSample> {
    DstSample sample_convert(const SrcSample&);
};
\endcode
*/
template <typename SrcSample, typename DstSample>
struct SampleConvertibleConcept
{
    void constraints() {
        gil_function_requires<SampleConcept<SrcSample> >();
        gil_function_requires<MutableSampleConcept<DstSample> >();
        dst = sample_convert<DstSample, SrcSample>(src);
	ignore_unused_variable_warning(dst);
    }
    SrcSample src;
    DstSample dst;
};


/**
 *
 *  Channel Base Concepts
 *
 */

/**
   \ingroup ChannelBaseConcept

   \brief A channel base is a container of channel elements (such as
   samples, sample references or sample pointers)
*/
/** 
    The most common use of channel base is in the implementation of a
    frame, in which case the channel elements are sample values. The
    channel base concept, however, can be used in other scenarios. For
    example, a planar frame has samples that are not contiguous in
    memory. Its reference is a proxy class that uses a channel base
    whose elements are sample references. Its iterator uses a channel
    base whose elements are sample iterators.

    A channel base must have an associated layout (which consists of a
    channel space, as well as an ordering of the samples).  There are
    two ways to index the elements of a channel base: A physical index
    corresponds to the way they are ordered in memory, and a semantic
    index corresponds to the way the elements are ordered in their
    channel space.  For example, in the RGB channel space the elements are
    ordered as {red_t, green_t, blue_t}. For a channel base with a BGR
    layout, the first element in physical ordering is the blue
    element, whereas the first semantic element is the red one.
    Models of \p ChannelBaseConcept are required to provide the \p
    at_c<K>(ChannelBase) function, which allows for accessing the
    elements based on their physical order. GIL provides a \p
    semantic_at_c<K>(ChannelBase) function (described later) which can
    operate on any model of ChannelBaseConcept and returns the
    corresponding semantic element.

\code
concept ChannelBaseConcept<typename T> :
      CopyConstructible<T>, EqualityComparable<T>
{
    // a GIL layout (the channel space and element permutation)
    typename layout_t;     
        
    // The type of K-th element
    template <int K> struct kth_element_type;
    where Metafunction<kth_element_type>;
    
    // The result of at_c
    template <int K> struct kth_element_const_reference_type;
    where Metafunction<kth_element_const_reference_type>;        
    
    template <int K> kth_element_const_reference_type<T,K>::type at_c(T);

    // Copy-constructible and equality comparable with other compatible channel bases
    template <ChannelBaseConcept T2> where { ChannelBasesCompatibleConcept<T,T2> } 
        T::T(T2);
    template <ChannelBaseConcept T2> where { ChannelBasesCompatibleConcept<T,T2> } 
        bool operator==(const T&, const T2&);
    template <ChannelBaseConcept T2> where { ChannelBasesCompatibleConcept<T,T2> } 
        bool operator!=(const T&, const T2&);

};
\endcode
*/

template <typename ChannelBase>
struct ChannelBaseConcept
{
    void constraints() {
        gil_function_requires< CopyConstructible<ChannelBase> >();
        gil_function_requires< EqualityComparable<ChannelBase> >();

        typedef typename ChannelBase::layout_t::channel_space_t channel_space_t;
        gil_function_requires<ChannelSpaceConcept<channel_space_t> >();

        typedef typename ChannelBase::layout_t::sample_mapping_t sample_mapping_t;
        // TODO: sample_mapping_t must be an MPL RandomAccessSequence

        static const std::size_t num_elements = size<ChannelBase>::value;

        typedef typename kth_element_type<ChannelBase,num_elements-1>::type TN; 
        typedef typename kth_element_const_reference_type<
	    ChannelBase,num_elements-1>::type CR; 

#if !defined(_MSC_VER) || _MSC_VER > 1310
        CR cr = at_c<num_elements-1>(cb);
	ignore_unused_variable_warning(cr);
#endif

        // functions that work for every frame (no need to require them)
        semantic_at_c<0>(cb);
        semantic_at_c<num_elements-1>(cb);
        // also static_max(cb), static_min(cb), static_fill(cb,value),
        // and all variations of static_for_each(), static_generate(),
        // static_transform()
    }

    ChannelBase cb;
};

/**
   \ingroup ChannelBaseConcept
   \brief Channel base which allows for modifying its elements
*/
/** 

\code
concept MutableChannelBaseConcept<ChannelBaseConcept T> :
           Assignable<T>, Swappable<T> {
    template <int K> struct kth_element_reference_type;
    where Metafunction<kth_element_reference_type>;

    template <int K>
    kth_element_reference_type<kth_element_type<T,K>::type>::type at_c(T);
    
    template <ChannelBaseConcept T2> where { ChannelBasesCompatibleConcept<T,T2> } 
        T& operator=(T&, const T2&);
};
\endcode
*/
template <typename ChannelBase>
struct MutableChannelBaseConcept
{
    void constraints() {
        gil_function_requires< ChannelBaseConcept<ChannelBase> >();
        gil_function_requires< Assignable<ChannelBase> >();
        gil_function_requires< Swappable<ChannelBase> >();

        typedef typename kth_element_reference_type<ChannelBase, 0>::type CR; 

#if !defined(_MSC_VER) || _MSC_VER > 1310
        CR r=at_c<0>(cb);
        at_c<0>(cb)=r;
#endif
    }

    ChannelBase cb;
};

/**
   \ingroup ChannelBaseConcept
   \brief Channel base that also has a default-constructor. Refines
   Regular
*/
/** 
\code
concept ChannelBaseValueConcept<typename T> :
    MutableChannelBaseConcept<T>, Regular<T> {
};
\endcode
*/
template <typename ChannelBase>
struct ChannelBaseValueConcept
{
    void constraints() {
        gil_function_requires< MutableChannelBaseConcept<ChannelBase> >();
        gil_function_requires< Regular<ChannelBase> >();
    }
};

/**
   \ingroup ChannelBaseConcept
   \brief Channel base whose elements all have the same type
*/
/** 
\code
concept HomogeneousChannelBaseConcept<ChannelBaseConcept CB> {
    // For all K in [0 ... size<C1>::value-1):
    //     where SameType<kth_element_type<CB,K>::type,
                             kth_element_type<CB,K+1>::type>;    
    kth_element_const_reference_type<CB,0>::type dynamic_at_c(
           const CB&, std::size_t n) const;
};
\endcode
*/
template <typename ChannelBase>
struct HomogeneousChannelBaseConcept
{
    void constraints() {
        gil_function_requires< ChannelBaseConcept<ChannelBase> >();

        static const std::size_t num_elements = size<ChannelBase>::value;

        typedef typename kth_element_type<ChannelBase,0>::type T0; 
        typedef typename kth_element_type<ChannelBase,num_elements-1>::type TN; 

        BOOST_STATIC_ASSERT((is_same<T0,TN>::value));   // better than nothing
        typedef typename kth_element_const_reference_type<ChannelBase,0>::type CRef0; 
        CRef0 e0=dynamic_at_c(cb,0);
    }
    ChannelBase cb;
};

/**
   \ingroup ChannelBaseConcept
   \brief Homogeneous channel base that allows for modifying its
   elements
*/
/** 

\code
concept MutableHomogeneousChannelBaseConcept<ChannelBaseConcept CB> :
            HomogeneousChannelBaseConcept<CB> {
    kth_element_reference_type<CB,0>::type dynamic_at_c(CB&, std::size_t n);
};
\endcode
*/

template <typename ChannelBase>
struct MutableHomogeneousChannelBaseConcept
{
    void constraints() {
        gil_function_requires< ChannelBaseConcept<ChannelBase> >();
        gil_function_requires< HomogeneousChannelBaseConcept<ChannelBase> >();
        typedef typename kth_element_reference_type<ChannelBase, 0>::type R0;
        R0 x = dynamic_at_c (cb, 0);
        dynamic_at_c(cb,0) = dynamic_at_c (cb, 0);
    }
    ChannelBase cb;
};

/**
   \ingroup ChannelBaseConcept
   \brief Homogeneous channel base that also has a default
   constructor. Refines Regular.
*/
/** 

\code
concept HomogeneousChannelBaseValueConcept<typename T> :
         MutableHomogeneousChannelBaseConcept<T>, Regular<T>
{ };
\endcode
*/

template <typename ChannelBase>
struct HomogeneousChannelBaseValueConcept
{
    void constraints() {
        gil_function_requires< MutableHomogeneousChannelBaseConcept<ChannelBase> >();
        gil_function_requires< Regular<ChannelBase> >();
    }
};


/**
   \ingroup ChannelBaseConcept
   \brief Two channel bases are compatible if they have the same channel
   space and their elements are compatible, semantic-pairwise.
*/
/** 

\code
concept ChannelBasesCompatibleConcept<ChannelBaseConcept C1, ChannelBaseConcept C2> {
    where SameType<C1::layout_t::channel_space_t, C2::layout_t::channel_space_t>;
    // also, for all K in [0 ... size<C1>::value):
    //     where Convertible<kth_semantic_element_type<C1,K>::type,
                 kth_semantic_element_type<C2,K>::type>;
    //     where Convertible<kth_semantic_element_type<C2,K>::type,
                 kth_semantic_element_type<C1,K>::type>;
};
\endcode
*/
template <typename ChannelBase1, typename ChannelBase2>
struct ChannelBasesCompatibleConcept {
    void constraints() {
        BOOST_STATIC_ASSERT(
	    (is_same<typename ChannelBase1::layout_t::channel_space_t, 
	     typename ChannelBase2::layout_t::channel_space_t>::value));
//        typedef typename kth_semantic_element_type<ChannelBase1,0>::type e1;
//        typedef typename kth_semantic_element_type<ChannelBase2,0>::type e2;
//        "e1 is convertible to e2"
    }
};


/*
 *
 *      Frame Concepts
 *
 */

/**
   \brief Concept for all frame-based GIL constructs, such as frames,
   iterators, locators, views and buffers whose value type is a frame
   
   \ingroup FrameBasedConcept
*/
/**
\code
concept FrameBasedConcept<typename T> {
    typename channel_space_type<T>;     
        where Metafunction<channel_space_type<T> >;
        where ChannelSpaceConcept<channel_space_type<T>::type>;
    typename sample_mapping_type<T>; 
        where Metafunction<sample_mapping_type<T> >;  
        where SampleMappingConcept<sample_mapping_type<T>::type>;
    typename is_planar<T>;
        where Metafunction<is_planar<T> >;
        where SameType<is_planar<T>::type, bool>;
};
\endcode
*/
template <typename P>
struct FrameBasedConcept
{
    void constraints() {
        typedef typename channel_space_type<P>::type channel_space_t;
        gil_function_requires<ChannelSpaceConcept<channel_space_t> >();
        typedef typename sample_mapping_type<P>::type sample_mapping_t;
        gil_function_requires<SampleMappingConcept<sample_mapping_t> >();

        static const bool planar = is_planar<P>::type::value;
	ignore_unused_variable_warning(planar);


        // This is not part of the concept, but should still work
        static const std::size_t nc = num_samples<P>::value;
        ignore_unused_variable_warning(nc);
    }
};

/**
   \brief Concept for homogeneous frame-based GIL constructs
   \ingroup FrameBasedConcept
*/
/**
\code
concept HomogeneousFrameBasedConcept<FrameBasedConcept T> {
    typename sample_type<T>;         
        where Metafunction<sample_type<T> >;
        where SampleConcept<sample_type<T>::type>;
};
\endcode
*/
template <typename P>
struct HomogeneousFrameBasedConcept
{
    void constraints() {
        gil_function_requires<FrameBasedConcept<P> >();
        typedef typename sample_type<P>::type sample_t;
        gil_function_requires<SampleConcept<sample_t> >();        
    }
};


/**
   \brief Frame concept - A channel base whose elements are samples
   \ingroup FrameConcept
*/
/**
\code
concept FrameConcept<typename P> : ChannelBaseConcept<P>, FrameBasedConcept<P> {    
    where is_frame<P>::type::value==true;
    // where for each K [0..size<P>::value-1]:
    //      SampleConcept<kth_element_type<P,K> >;
        
    typename P::value_type;       where FrameValueConcept<value_type>;
    typename P::reference;        where FrameConcept<reference>;
    typename P::const_reference;  where FrameConcept<const_reference>;
    static const bool P::is_mutable;

    template <FrameConcept P2> where { FrameConcept<P,P2> } 
        P::P(P2);
    template <FrameConcept P2> where { FrameConcept<P,P2> } 
        bool operator==(const P&, const P2&);
    template <FrameConcept P2> where { FrameConcept<P,P2> } 
        bool operator!=(const P&, const P2&);
}; 
\endcode
*/

template <typename P>
struct FrameConcept
{
    void constraints() {
        gil_function_requires<ChannelBaseConcept<P> >();
        gil_function_requires<FrameBasedConcept<P> >();

        BOOST_STATIC_ASSERT((is_frame<P>::value));
        static const bool is_mutable = P::is_mutable;
	ignore_unused_variable_warning(is_mutable);

        typedef typename P::value_type      value_type;
//      gil_function_requires<FrameValueConcept<value_type> >();

        typedef typename P::reference       reference;
        gil_function_requires<
	    FrameConcept<typename remove_const_and_reference<reference>::type> >();

        typedef typename P::const_reference const_reference;
        gil_function_requires<
	    FrameConcept<
		typename remove_const_and_reference<const_reference>::type> >();
    }
};


/**
   \brief Frame concept that allows for changing its samples
   \ingroup FrameConcept
*/
/**
\code
concept MutableFrameConcept<FrameConcept P> : MutableChannelBaseConcept<P> {
    where is_mutable==true;
};
\endcode
*/
template <typename P>
struct MutableFrameConcept
{
    void constraints() {
        gil_function_requires<FrameConcept<P> >();
        BOOST_STATIC_ASSERT(P::is_mutable);
    }
};

/**
   \brief Homogeneous frame concept
   \ingroup FrameConcept
*/
/**
\code
concept HomogeneousFrameConcept<FrameConcept P> :
   HomogeneousChannelBaseConcept<P>, HomogeneousFrameBasedConcept<P> { 
     P::template element_const_reference_type<P>::type operator[] (
          P p, std::size_t i) const { return dynamic_at_c(p,i); }
};
\endcode
*/
template <typename P>
struct HomogeneousFrameConcept
{
    void constraints() {
        gil_function_requires<FrameConcept<P> >();
        gil_function_requires<HomogeneousChannelBaseConcept<P> >();
        gil_function_requires<HomogeneousFrameBasedConcept<P> >();
        p[0];
    }
    P p;
};

/**
   \brief Homogeneous frame concept that allows for changing its samples
   \ingroup FrameConcept
*/
/**
\code
concept MutableHomogeneousFrameConcept<HomogeneousFrameConcept P> :
           MutableHomogeneousChannelBaseConcept<P> { 
    P::template element_reference_type<P>::type operator[](P p,
    std::size_t i) {
     return dynamic_at_c(p,i); }
};
\endcode
*/
template <typename P>
struct MutableHomogeneousFrameConcept
{
    void constraints() {
        gil_function_requires<HomogeneousFrameConcept<P> >();
        gil_function_requires<MutableHomogeneousChannelBaseConcept<P> >();
        p[0] = p[0];
    }
    P p;
};

/**
   \brief Frame concept that is a Regular type
   \ingroup FrameConcept
*/
/**
\code
concept FrameValueConcept<FrameConcept P> : Regular<P> {
    where SameType<value_type,P>;
};    
\endcode
*/
template <typename P>
struct FrameValueConcept {
    void constraints() {
        gil_function_requires<FrameConcept<P> >();
        gil_function_requires<Regular<P> >();
    }
};

/**
   \brief Homogeneous frame concept that is a Regular type
   \ingroup FrameConcept
*/
/**
\code
concept HomogeneousFrameValueConcept<HomogeneousFrameConcept P> : Regular<P> {
    where SameType<value_type,P>;
}; 
\endcode
*/
template <typename P>
struct HomogeneousFrameValueConcept
{
    void constraints() {
        gil_function_requires<HomogeneousFrameConcept<P> >();
        gil_function_requires<Regular<P> >();
        BOOST_STATIC_ASSERT((is_same<P, typename P::value_type>::value));
    }
};

namespace detail
{
    template <typename P1, typename P2, int K>
    struct samples_are_pairwise_compatible : public 
        mpl::and_<samples_are_pairwise_compatible<P1,P2,K-1>,
		  samples_are_compatible<
		      typename kth_semantic_element_reference_type<P1,K>::type,
		      typename kth_semantic_element_reference_type<P2,K>::type> >
    {};
                                                 
    template <typename P1, typename P2>
    struct samples_are_pairwise_compatible<P1,P2,-1> : public mpl::true_ {};
} /* namespace detail */

/**
   \brief Returns whether two frames are compatible

   Frames are compatible if their samples and channel space types are
   compatible. Compatible frames can be assigned and copy constructed
   from one another.
   
   \ingroup FrameAlgorithm
*/
template <typename P1, typename P2>  // Models GIL Frame
struct frames_are_compatible 
    : public mpl::and_<typename channel_spaces_are_compatible<
			   typename channel_space_type<P1>::type, 
			   typename channel_space_type<P2>::type>::type, 
                       detail::samples_are_pairwise_compatible<
			   P1, P2, num_samples<P1>::value-1> > {};

/**
   \brief  Concept for frame compatibility

   Frames are compatible if their samples and channel space types are
   compatible. Compatible frames can be assigned and copy constructed
   from one another.
   
   \ingroup FrameConcept
*/
/**
\code
concept FramesCompatibleConcept<FrameConcept P1, FrameConcept P2> :
          ChannelBasesCompatibleConcept<P1,P2> {
    // where for each K [0..size<P1>::value):
    //    SamplesCompatibleConcept<kth_semantic_element_type<P1,K>::type,
                kth_semantic_element_type<P2,K>::type>;
};
\endcode
*/
template <typename P1, typename P2>
// precondition: P1 and P2 model FrameConcept
struct FramesCompatibleConcept
{
    void constraints() {
        BOOST_STATIC_ASSERT((frames_are_compatible<P1,P2>::value));
    }
};

/**
   \brief Frame convertible concept

   Convertibility is non-symmetric and implies that one frame can be
   converted to another, approximating the channel. Conversion is
   explicit and sometimes lossy.
   \ingroup FrameConcept
*/
/**
\code
template <FrameConcept SrcFrame, MutableFrameConcept DstFrame>
concept FrameConvertibleConcept {
    void channel_convert(const SrcFrame&, DstFrame&);
};
\endcode
*/
template <typename SrcP, typename DstP>
struct FrameConvertibleConcept
{
    void constraints() {
        gil_function_requires<FrameConcept<SrcP> >();
        gil_function_requires<MutableFrameConcept<DstP> >();
        channel_convert(src,dst);
    }
    SrcP src;
    DstP dst;
};

/*
 *
 *      Dereference Adaptors Concepts
 *
 */

/**
   \ingroup FrameDereferenceAdaptorConcept

   \brief Represents a unary function object that can be invoked upon
   dereferencing a frame iterator.
   
   This can perform an arbitrary computation, such as channel conversion
   or table lookup
*/
/**
\code
concept FrameDereferenceAdaptorConcept<boost::UnaryFunctionConcept D>
  : DefaultConstructibleConcept<D>, CopyConstructibleConcept<D>,
    AssignableConcept<D>  {
    typename const_t;         where FrameDereferenceAdaptorConcept<const_t>;
    typename value_type;      where FrameValueConcept<value_type>;
    typename reference;         // may be mutable
    typename const_reference;   // must not be mutable
    static const bool D::is_mutable;

    where Convertible<value_type,result_type>;
};
\endcode
*/

template <typename D>
struct FrameDereferenceAdaptorConcept
{
    void constraints() {
        gil_function_requires< boost::UnaryFunctionConcept<D, 
            typename remove_const_and_reference<typename D::result_type>::type, 
            typename D::argument_type> >();
        gil_function_requires< boost::DefaultConstructibleConcept<D> >();
        gil_function_requires< boost::CopyConstructibleConcept<D> >();
        gil_function_requires< boost::AssignableConcept<D> >();

        gil_function_requires<FrameConcept<
	    typename remove_const_and_reference<
		typename D::result_type>::type> >();

        typedef typename D::const_t const_t;
        gil_function_requires<FrameDereferenceAdaptorConcept<const_t> >();
        typedef typename D::value_type value_type;
        gil_function_requires<FrameValueConcept<value_type> >();
        typedef typename D::reference reference;
	// == FrameConcept (if you remove const and reference)
        typedef typename D::const_reference const_reference;
	// == FrameConcept (if you remove const and reference)

        const bool is_mutable = D::is_mutable;
	ignore_unused_variable_warning (is_mutable);
    }
    D d;
};

template <typename P>
struct FrameDereferenceAdaptorArchetype : public std::unary_function<P, P>
{
    typedef FrameDereferenceAdaptorArchetype const_t;
    typedef typename remove_reference<P>::type value_type;
    typedef typename add_reference<P>::type reference;
    typedef reference const_reference;
    static const bool is_mutable=false;
    P operator()(P x) const { throw; }
};


/*
 *
 *     Frame iterator concepts.
 *
 */

/**
   \brief Concept for iterators, locators and views that can define a
   type just like the given iterator/locator/view, except it supports
   runtime specified step along the X navigation
   
   \ingroup FrameIteratorConcept
*/
/**
\code
concept HasDynamicXStepTypeConcept<typename T> {
    typename dynamic_x_step_type<T>;
        where Metafunction<dynamic_x_step_type<T> >;
};
\endcode
*/
template <typename T>
struct HasDynamicXStepTypeConcept
{   
    void constraints() {
        typedef typename dynamic_x_step_type<T>::type type;
    }
};

/**
   \brief Concept for locators and views that can define a type just
   like the given locator or view, except it supports runtime
   specified step along the Y navigation
   
   \ingroup FrameLocatorConcept
*/
/**
\code
concept HasDynamicYStepTypeConcept<typename T> {
    typename dynamic_y_step_type<T>;
        where Metafunction<dynamic_y_step_type<T> >;
};
\endcode
*/
template <typename T>
struct HasDynamicYStepTypeConcept
{   
    void constraints() {
        typedef typename dynamic_y_step_type<T>::type type;
    }
};


/**
   \brief Concept for locators and views that can define a type just
   like the given locator or view, except X and Y is swapped
   
   \ingroup FrameLocatorConcept
*/
/**
\code
concept HasTransposedTypeConcept<typename T> {
    typename transposed_type<T>;
        where Metafunction<transposed_type<T> >;
};
\endcode
*/
template <typename T>
struct HasTransposedTypeConcept
{   
    void constraints() {
        typedef typename transposed_type<T>::type type;
    }
};

/**
   \defgroup FrameIteratorConceptFrameIterator FrameIteratorConcept
   \ingroup FrameIteratorConcept
   \brief STL iterator over frames
   
   \ingroup FrameIteratorConceptFrameIterator
   \brief An STL random access traversal iterator over a model of
   FrameConcept.
*/
/**
   GIL's iterators must also provide the following metafunctions:
 - \p const_iterator_type<Iterator>:
   Returns a read-only equivalent of \p Iterator
 - \p iterator_is_mutable<Iterator>:
   Returns whether the given iterator is read-only or mutable
 - \p is_iterator_adaptor<Iterator>:
   Returns whether the given iterator is an adaptor over another
   iterator. See IteratorAdaptorConcept for additional requirements of
   adaptors.

 \code
concept FrameIteratorConcept<typename Iterator> :
boost_concepts::RandomAccessTraversalConcept<Iterator>,
FrameBasedConcept<Iterator> {
    where FrameValueConcept<value_type>;
    typename const_iterator_type<It>::type;         
        where FrameIteratorConcept<const_iterator_type<It>::type>;
    static const bool  iterator_is_mutable<It>::type::value;          
    static const bool  is_iterator_adaptor<It>::type::value;
// is it an iterator adaptor
};
\endcode
*/
template <typename Iterator>
struct FrameIteratorConcept
{   
    void constraints()
    {
        gil_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<Iterator> >();
        gil_function_requires<FrameBasedConcept<Iterator> >();
        
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
        gil_function_requires<FrameValueConcept<value_type> >();
 
        typedef typename const_iterator_type<Iterator>::type const_t;
        static const bool is_mut = iterator_is_mutable<Iterator>::type::value;
	ignore_unused_variable_warning (is_mut);

        const_t const_it(it);
	ignore_unused_variable_warning(const_it);
        // immutable iterator must be constructible from (possibly
	// mutable) iterator

        check_base (typename is_iterator_adaptor<Iterator>::type ());
    }
    
    void check_base(mpl::false_) {}
    void check_base(mpl::true_)
    {
        typedef typename iterator_adaptor_get_base<Iterator>::type base_t;
        gil_function_requires<FrameIteratorConcept<base_t> >();
    }

    Iterator it;
};

namespace detail
{
    template <typename Iterator>
    // Preconditions: Iterator Models FrameIteratorConcept
    struct FrameIteratorIsMutableConcept
    {
        void constraints()
	{
            gil_function_requires<
		detail::RandomAccessIteratorIsMutableConcept<Iterator> >();
            typedef typename remove_reference<
		typename std::iterator_traits<Iterator>::reference>::type ref;
            typedef typename element_type<ref>::type sample_t;
            gil_function_requires<detail::SampleIsMutableConcept<sample_t> >();
        }
    };
}

/**
   \brief Frame iterator that allows for changing its frame
   \ingroup FrameIteratorConceptFrameIterator
*/
/**
\code
concept MutableFrameIteratorConcept<FrameIteratorConcept Iterator> :
          MutableRandomAccessIteratorConcept<Iterator> {};

\endcode
*/
template <typename Iterator>
struct MutableFrameIteratorConcept
{
    void constraints()
    {
        gil_function_requires<FrameIteratorConcept<Iterator> >();
        gil_function_requires<detail::FrameIteratorIsMutableConcept<Iterator> >();
    }
};

namespace detail {
/*
  Iterators that can be used as the base of memory_based_step_iterator
  require some additional functions
*/
template <typename Iterator>
// Preconditions: Iterator Models boost_concepts::RandomAccessTraversalConcept
struct RandomAccessIteratorIsMemoryBasedConcept
{
    void constraints()
    {
	std::ptrdiff_t bs=memunit_step(it);  ignore_unused_variable_warning(bs);
	it=memunit_advanced(it,3);
	std::ptrdiff_t bd=memunit_distance(it,it);
	ignore_unused_variable_warning(bd);
	memunit_advance(it,3);
	// for performace you may also provide a customized
	// implementation of memunit_advanced_ref
    }
    Iterator it;
};
} /* namespace detail */

/**
   \defgroup FrameIteratorConceptStepIterator StepIteratorConcept
   \ingroup FrameIteratorConcept
   \brief Iterator that advances by a specified step

   \brief Concept of a random-access iterator that can be advanced in
   memory units (bytes or bits)
   \ingroup FrameIteratorConceptStepIterator
*/
/**
\code
concept MemoryBasedIteratorConcept<
          boost_concepts::RandomAccessTraversalConcept Iterator> {
    typename byte_to_memunit<Iterator>;
    where metafunction<byte_to_memunit<Iterator> >;
    std::ptrdiff_t      memunit_step(const Iterator&);
    std::ptrdiff_t      memunit_distance(const Iterator& , const Iterator&);
    void                memunit_advance(Iterator&, std::ptrdiff_t diff);
    Iterator            memunit_advanced(const Iterator& p,
    std::ptrdiff_t diff)
    { Iterator tmp; memunit_advance(tmp,diff); return tmp; }
    Iterator::reference memunit_advanced_ref(
    const Iterator& p, std::ptrdiff_t diff) { return *memunit_advanced(p,diff); }
};
\endcode
*/
template <typename Iterator>
struct MemoryBasedIteratorConcept
{
    void constraints ()
    {
        gil_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<Iterator> >();
        gil_function_requires<
	    detail::RandomAccessIteratorIsMemoryBasedConcept<Iterator> >();
    }
};

/**
   \brief Step iterator concept

   Step iterators are iterators that have a set_step method
   \ingroup FrameIteratorConceptStepIterator
*/
/**
\code
concept StepIteratorConcept<boost_concepts::ForwardTraversalConcept Iterator> {
    template <Integral D> void Iterator::set_step(D step);
};
\endcode
*/
template <typename Iterator>
struct StepIteratorConcept
{
    void constraints()
    {
        gil_function_requires<
	    boost_concepts::ForwardTraversalConcept<Iterator> >();
        it.set_step(0);
    }
    Iterator it;
};


/**
   \brief Step iterator that allows for modifying its current value
   \ingroup FrameIteratorConceptStepIterator
*/
/**
\code
concept MutableStepIteratorConcept<
    Mutable_ForwardIteratorConcept Iterator> : StepIteratorConcept<Iterator> {};
\endcode
*/
template <typename Iterator>
struct MutableStepIteratorConcept
{
    void constraints()
    {
        gil_function_requires<StepIteratorConcept<Iterator> >();
        gil_function_requires<
	    detail::ForwardIteratorIsMutableConcept<Iterator> >();
    }
};

/**
   \defgroup FrameIteratorConceptIteratorAdaptor IteratorAdaptorConcept
   \ingroup FrameIteratorConcept
   \brief Adaptor over another iterator

   \ingroup FrameIteratorConceptIteratorAdaptor
   \brief Iterator adaptor is a forward iterator adapting another
   forward iterator.
*/
/**
   In addition to GIL iterator requirements, GIL iterator adaptors
   must provide the following metafunctions:
 - \p is_iterator_adaptor<Iterator>:             Returns \p mpl::true_
 - \p iterator_adaptor_get_base<Iterator>:       Returns the base iterator type
 - \p iterator_adaptor_rebind<Iterator,NewBase>: Replaces the base
                                                 iterator with the new one
   
   The adaptee can be obtained from the iterator via the "base()"
   method.
   
\code
concept IteratorAdaptorConcept<boost_concepts::ForwardTraversalConcept Iterator> {
    where SameType<is_iterator_adaptor<Iterator>::type, mpl::true_>;

    typename iterator_adaptor_get_base<Iterator>;
        where Metafunction<iterator_adaptor_get_base<Iterator> >;
        where boost_concepts::ForwardTraversalConcept<iterator_adaptor_get_base<Iterator>::type>;
    
    typename another_iterator; 
    typename iterator_adaptor_rebind<Iterator,another_iterator>::type;
        where boost_concepts::ForwardTraversalConcept<another_iterator>;
        where IteratorAdaptorConcept<iterator_adaptor_rebind<Iterator,another_iterator>::type>;

    const iterator_adaptor_get_base<Iterator>::type& Iterator::base() const;
};
\endcode
*/
template <typename Iterator>
struct IteratorAdaptorConcept
{
    void constraints()
    {
        gil_function_requires<
	    boost_concepts::ForwardTraversalConcept<Iterator> >();

        typedef typename iterator_adaptor_get_base<Iterator>::type base_t;
        gil_function_requires<boost_concepts::ForwardTraversalConcept<base_t> >();

        BOOST_STATIC_ASSERT(is_iterator_adaptor<Iterator>::value);
        typedef typename iterator_adaptor_rebind<Iterator, void*>::type rebind_t;

        base_t base=it.base();  ignore_unused_variable_warning(base);
    }
    Iterator it;
};

/**
   \brief Iterator adaptor that is mutable
   \ingroup FrameIteratorConceptIteratorAdaptor
*/
/**
\code
concept MutableIteratorAdaptorConcept<
    Mutable_ForwardIteratorConcept Iterator> :
        IteratorAdaptorConcept<Iterator> {};
\endcode
*/
template <typename Iterator>
struct MutableIteratorAdaptorConcept
{
    void constraints()
    {
        gil_function_requires<IteratorAdaptorConcept<Iterator> >();
        gil_function_requires<
	    detail::ForwardIteratorIsMutableConcept<Iterator> >();
    }
};


////////////////////////////////////////////////////////////////////////////////////////
///
///         BUFFER VIEW CONCEPTS
///
////////////////////////////////////////////////////////////////////////////////////////

/// \defgroup BufferViewNDConcept BufferViewNDLocatorConcept
/// \ingroup BufferViewConcept
/// \brief N-dimensional range

/// \defgroup BufferView2DConcept BufferView2DConcept
/// \ingroup BufferViewConcept
/// \brief 2-dimensional range

/// \defgroup FrameBufferViewConcept BufferViewConcept
/// \ingroup BufferViewConcept
/// \brief 2-dimensional range over frame data

/// \ingroup BufferViewNDConcept
/// \brief N-dimensional view over immutable values
/**
\code
concept RandomAccessNDBufferViewConcept<Regular View> {
    typename value_type;
    typename reference;       // result of dereferencing
    typename difference_type; // result of operator-(iterator,iterator) (1-dimensional!)
    typename const_t;  where RandomAccessNDBufferViewConcept<View>; // same as View, but over immutable values
    typename point_t;  where PointNDConcept<point_t>; // N-dimensional point
    typename locator;  where RandomAccessNDLocatorConcept<locator>; // N-dimensional locator.
    typename iterator; where RandomAccessTraversalConcept<iterator>; // 1-dimensional iterator over all values
    typename reverse_iterator; where RandomAccessTraversalConcept<reverse_iterator>; 
    typename size_type;       // the return value of size()

    // Equivalent to RandomAccessNDLocatorConcept::axis
    template <size_t D> struct axis {
        typename coord_t = point_t::axis<D>::coord_t;
        typename iterator; where RandomAccessTraversalConcept<iterator>;   // iterator along D-th axis.
        where SameType<coord_t, iterator::difference_type>;
        where SameType<iterator::value_type,value_type>;
    };

    // Defines the type of a view similar to this type, except it invokes Deref upon dereferencing
    template <FrameDereferenceAdaptorConcept Deref> struct add_deref {
        typename type;        where RandomAccessNDBufferViewConcept<type>;
        static type make(const View& v, const Deref& deref);
    };

    static const size_t num_dimensions = point_t::num_dimensions;
    
    // Create from a locator at the top-left corner and dimensions
    View::View(const locator&, const point_type&);
    
    size_type        View::size()       const; // total number of elements
    reference        operator[](View, const difference_type&) const; // 1-dimensional reference
    iterator         View::begin()      const;
    iterator         View::end()        const;
    reverse_iterator View::rbegin()     const;
    reverse_iterator View::rend()       const;
    iterator         View::at(const point_t&);
    point_t          View::dimensions() const; // number of elements along each dimension
    bool             View::is_1d_traversable() const;   // can an iterator over the first dimension visit each value? I.e. are there gaps between values?

    // iterator along a given dimension starting at a given point
    template <size_t D> View::axis<D>::iterator View::axis_iterator(const point_t&) const;

    reference operator()(View,const point_t&) const;
};
\endcode
*/
template <typename View>
struct RandomAccessNDBufferViewConcept {
    void constraints() {
        gil_function_requires< Regular<View> >();

        typedef typename View::value_type       value_type;
        typedef typename View::reference        reference;       // result of dereferencing
        typedef typename View::difference_type  difference_type; // result of operator-(1d_iterator,1d_iterator)
        typedef typename View::const_t          const_t;         // same as this type, but over const values
        typedef typename View::point_t          point_t;         // N-dimensional point
        typedef typename View::locator          locator;         // N-dimensional locator
        typedef typename View::iterator         iterator;
        typedef typename View::reverse_iterator reverse_iterator;
        typedef typename View::size_type        size_type;
        static const std::size_t N=View::num_dimensions;
    
        gil_function_requires<RandomAccessNDLocatorConcept<locator> >();
        gil_function_requires<boost_concepts::RandomAccessTraversalConcept<iterator> >();
        gil_function_requires<boost_concepts::RandomAccessTraversalConcept<reverse_iterator> >();

        typedef typename View::template axis<0>::iterator   first_it_type;
        typedef typename View::template axis<N-1>::iterator last_it_type;
        gil_function_requires<boost_concepts::RandomAccessTraversalConcept<first_it_type> >();
        gil_function_requires<boost_concepts::RandomAccessTraversalConcept<last_it_type> >();

//        BOOST_STATIC_ASSERT((typename std::iterator_traits<first_it_type>::difference_type, typename point_t::template axis<0>::coord_t>::value));
//        BOOST_STATIC_ASSERT((typename std::iterator_traits< last_it_type>::difference_type, typename point_t::template axis<N-1>::coord_t>::value));

        // point_t must be an N-dimensional point, each dimension of which must have the same type as difference_type of the corresponding iterator
        gil_function_requires<PointNDConcept<point_t> >();
        BOOST_STATIC_ASSERT(point_t::num_dimensions==N);
        BOOST_STATIC_ASSERT((is_same<typename std::iterator_traits<first_it_type>::difference_type, typename point_t::template axis<0>::coord_t>::value));
        BOOST_STATIC_ASSERT((is_same<typename std::iterator_traits<last_it_type>::difference_type, typename point_t::template axis<N-1>::coord_t>::value));

        point_t p;
        locator lc;
        iterator it;
        reverse_iterator rit;
        difference_type d; detail::initialize_it(d); ignore_unused_variable_warning(d);

        View(p,lc); // view must be constructible from a locator and a point

        p=view.dimensions();
        lc=view.frames();
        size_type sz=view.size();  ignore_unused_variable_warning(sz);
        bool is_contiguous=view.is_1d_traversable(); ignore_unused_variable_warning(is_contiguous);

        it=view.begin();
        it=view.end();
        rit=view.rbegin();
        rit=view.rend();

        reference r1=view[d]; ignore_unused_variable_warning(r1);    // 1D access 
        reference r2=view(p); ignore_unused_variable_warning(r2);    // 2D access

        // get 1-D iterator of any dimension at a given frame location
        first_it_type fi=view.template axis_iterator<0>(p); ignore_unused_variable_warning(fi);
        last_it_type li=view.template axis_iterator<N-1>(p); ignore_unused_variable_warning(li);

        typedef FrameDereferenceAdaptorArchetype<typename View::value_type> deref_t;
        typedef typename View::template add_deref<deref_t>::type dtype;
    }
    View view;
};

/// \ingroup BufferView2DConcept
/// \brief 2-dimensional view over immutable values
/**
\code
concept RandomAccess2DBufferViewConcept<RandomAccessNDBufferViewConcept View> {
    where num_dimensions==2;

    typename x_iterator = axis<0>::iterator;
    typename y_iterator = axis<1>::iterator;
    typename x_coord_t  = axis<0>::coord_t;
    typename y_coord_t  = axis<1>::coord_t;
    typename xy_locator = locator;
    
    x_coord_t View::width()  const;
    y_coord_t View::height() const;
    
    // X-navigation
    x_iterator View::x_at(const point_t&) const;
    x_iterator View::row_begin(y_coord_t) const;
    x_iterator View::row_end  (y_coord_t) const;

    // Y-navigation
    y_iterator View::y_at(const point_t&) const;
    y_iterator View::col_begin(x_coord_t) const;
    y_iterator View::col_end  (x_coord_t) const;
       
    // navigating in 2D
    xy_locator View::xy_at(const point_t&) const;

    // (x,y) versions of all methods taking point_t    
    View::View(x_coord_t,y_coord_t,const locator&);
    iterator View::at(x_coord_t,y_coord_t) const;
    reference operator()(View,x_coord_t,y_coord_t) const;
    xy_locator View::xy_at(x_coord_t,y_coord_t) const;
    x_iterator View::x_at(x_coord_t,y_coord_t) const;
    y_iterator View::y_at(x_coord_t,y_coord_t) const;
};
\endcode
*/
template <typename View>
struct RandomAccess2DBufferViewConcept {
    void constraints() {
        gil_function_requires<RandomAccessNDBufferViewConcept<View> >();
        BOOST_STATIC_ASSERT(View::num_dimensions==2);

        // TODO: This executes the requirements for RandomAccessNDLocatorConcept again. Fix it to improve compile time
        gil_function_requires<RandomAccess2DLocatorConcept<typename View::locator> >();

        typedef typename dynamic_x_step_type<View>::type  dynamic_x_step_t;
        typedef typename dynamic_y_step_type<View>::type  dynamic_y_step_t;
        typedef typename transposed_type<View>::type      transposed_t;

        typedef typename View::x_iterator x_iterator;
        typedef typename View::y_iterator y_iterator;
        typedef typename View::x_coord_t  x_coord_t;
        typedef typename View::y_coord_t  y_coord_t;
        typedef typename View::xy_locator xy_locator;

        x_coord_t xd=0; ignore_unused_variable_warning(xd);
        y_coord_t yd=0; ignore_unused_variable_warning(yd);
        x_iterator xit;
        y_iterator yit;
        typename View::point_t d;

        View(xd,yd,xy_locator());       // constructible with width, height, 2d_locator

        xy_locator lc=view.xy_at(xd,yd);
        lc=view.xy_at(d);

        typename View::reference r=view(xd,yd);  ignore_unused_variable_warning(r);
        xd=view.width();
        yd=view.height();

        xit=view.x_at(d);
        xit=view.x_at(xd,yd);
        xit=view.row_begin(xd);
        xit=view.row_end(xd);

        yit=view.y_at(d);
        yit=view.y_at(xd,yd);
        yit=view.col_begin(xd);
        yit=view.col_end(xd);
    }
    View view;
};


/// \ingroup FrameBufferViewConcept
/// \brief GIL's 2-dimensional view over immutable GIL frames
/**
\code
concept BufferViewConcept<RandomAccess2DBufferViewConcept View> {
    where FrameValueConcept<value_type>;
    where FrameIteratorConcept<x_iterator>;        
    where FrameIteratorConcept<y_iterator>;
    where x_coord_t == y_coord_t;
    
    typename coord_t = x_coord_t;

    std::size_t View::num_samples() const;
};
\endcode
*/
template <typename View>
struct BufferViewConcept {
    void constraints() {
        gil_function_requires<RandomAccess2DBufferViewConcept<View> >();

        // TODO: This executes the requirements for RandomAccess2DLocatorConcept again. Fix it to improve compile time
        gil_function_requires<FrameLocatorConcept<typename View::xy_locator> >();
        
        BOOST_STATIC_ASSERT((is_same<typename View::x_coord_t, typename View::y_coord_t>::value));

        typedef typename View::coord_t           coord_t;      // 1D difference type (same for all dimensions)
        std::size_t num_chan = view.num_samples(); ignore_unused_variable_warning(num_chan);
    }
    View view;
};


namespace detail {
    template <typename View>    // Preconditions: View Models RandomAccessNDBufferViewConcept
    struct RandomAccessNDBufferViewIsMutableConcept {
        void constraints() {
            gil_function_requires<detail::RandomAccessNDLocatorIsMutableConcept<typename View::locator> >();

            gil_function_requires<detail::RandomAccessIteratorIsMutableConcept<typename View::iterator> >();
            gil_function_requires<detail::RandomAccessIteratorIsMutableConcept<typename View::reverse_iterator> >();
            gil_function_requires<detail::RandomAccessIteratorIsMutableConcept<typename View::template axis<0>::iterator> >();
            gil_function_requires<detail::RandomAccessIteratorIsMutableConcept<typename View::template axis<View::num_dimensions-1>::iterator> >();

            typename View::difference_type diff; initialize_it(diff); ignore_unused_variable_warning(diff);
            typename View::point_t pt;
            typename View::value_type v; initialize_it(v);

            view[diff]=v;
            view(pt)=v;
        }
        View view;
    };

    template <typename View>    // preconditions: View Models RandomAccessNDBufferViewConcept
    struct RandomAccess2DBufferViewIsMutableConcept {
        void constraints() {        
            gil_function_requires<detail::RandomAccessNDBufferViewIsMutableConcept<View> >();
            typename View::x_coord_t xd=0; ignore_unused_variable_warning(xd);
            typename View::y_coord_t yd=0; ignore_unused_variable_warning(yd);
            typename View::value_type v; initialize_it(v);
            view(xd,yd)=v;
        }
        View view;
    };

    template <typename View>    // preconditions: View Models BufferViewConcept
    struct FrameBufferViewIsMutableConcept {
        void constraints() {        
            gil_function_requires<detail::RandomAccess2DBufferViewIsMutableConcept<View> >();
        }
    };
}

/// \ingroup BufferViewNDConcept
/// \brief N-dimensional view over mutable values
/**
\code
concept MutableRandomAccessNDBufferViewConcept<RandomAccessNDBufferViewConcept View> {
    where Mutable<reference>;
};
\endcode
*/
template <typename View>
struct MutableRandomAccessNDBufferViewConcept {
    void constraints() {
        gil_function_requires<RandomAccessNDBufferViewConcept<View> >();
        gil_function_requires<detail::RandomAccessNDBufferViewIsMutableConcept<View> >();
    }
};

/// \ingroup BufferView2DConcept
/// \brief 2-dimensional view over mutable values
/**
\code
concept MutableRandomAccess2DBufferViewConcept<RandomAccess2DBufferViewConcept View> : MutableRandomAccessNDBufferViewConcept<View> {};
\endcode
*/
template <typename View>
struct MutableRandomAccess2DBufferViewConcept {
    void constraints() {
        gil_function_requires<RandomAccess2DBufferViewConcept<View> >();
        gil_function_requires<detail::RandomAccess2DBufferViewIsMutableConcept<View> >();
    }
};

/// \ingroup FrameBufferViewConcept
/// \brief GIL's 2-dimensional view over mutable GIL frames
/**
\code
concept MutableBufferViewConcept<BufferViewConcept View> : MutableRandomAccess2DBufferViewConcept<View> {};
\endcode
*/
template <typename View>
struct MutableBufferViewConcept {
    void constraints() {
        gil_function_requires<BufferViewConcept<View> >();
        gil_function_requires<detail::FrameBufferViewIsMutableConcept<View> >();
    }
};

/// \brief Returns whether two views are compatible
///
/// Views are compatible if their frames are compatible. Compatible views can be assigned and copy constructed from one another.
template <typename V1, typename V2>  // Model BufferViewConcept
struct views_are_compatible : public frames_are_compatible<typename V1::value_type, typename V2::value_type> {};

/// \brief Views are compatible if they have the same channel spaces and compatible sample values. Constness and layout are not important for compatibility
/// \ingroup BufferViewConcept
/**
\code
concept ViewsCompatibleConcept<BufferViewConcept V1, BufferViewConcept V2> {
    where FramesCompatibleConcept<V1::value_type, P2::value_type>;
};
\endcode
*/
template <typename V1, typename V2>
struct ViewsCompatibleConcept {
    void constraints() {
        BOOST_STATIC_ASSERT((views_are_compatible<V1,V2>::value));
    }
};


////////////////////////////////////////////////////////////////////////////////////////
///
///         BUFFER CONCEPTS
///
////////////////////////////////////////////////////////////////////////////////////////


/// \ingroup BufferConcept
/// \brief N-dimensional container of values
/**
\code
concept RandomAccessNDBufferConcept<typename Buf> : Regular<Buf> {
    typename view_t; where MutableRandomAccessNDBufferViewConcept<view_t>;
    typename const_view_t = view_t::const_t;
    typename point_t      = view_t::point_t;
    typename value_type   = view_t::value_type;
    typename allocator_type;

    Buf::Buf(point_t dims, std::size_t alignment=1);
    Buf::Buf(point_t dims, value_type fill_value, std::size_t alignment);
    
    void Buf::recreate(point_t new_dims, std::size_t alignment=1);
    void Buf::recreate(point_t new_dims, value_type fill_value, std::size_t alignment);

    const point_t&        Buf::dimensions() const;
    const const_view_t&   const_view(const Buf&);
    const view_t&         view(Buf&);
};
\endcode
*/
template <typename Buf>
struct RandomAccessNDBufferConcept {
    void constraints() {
        gil_function_requires<Regular<Buf> >();

        typedef typename Buf::view_t       view_t;
        gil_function_requires<MutableRandomAccessNDBufferViewConcept<view_t> >();

        typedef typename Buf::const_view_t const_view_t;
        typedef typename Buf::value_type   frame_t;

        typedef typename Buf::point_t        point_t;
        gil_function_requires<PointNDConcept<point_t> >();

        const_view_t cv = const_view(buf); ignore_unused_variable_warning(cv);
        view_t       v  = view(buf);       ignore_unused_variable_warning(v);

        frame_t fill_value;
        point_t pt=buf.dimensions();
        Buf im1(pt);
        Buf im2(pt,1);
        Buf im3(pt,fill_value,1);
        buf.recreate(pt);
        buf.recreate(pt,1);
        buf.recreate(pt,fill_value,1);
    }
    Buf buf;
};


/// \ingroup BufferConcept
/// \brief 2-dimensional container of values
/**
\code
concept RandomAccess2DBufferConcept<RandomAccessNDBufferConcept Buf> {
    typename x_coord_t = const_view_t::x_coord_t;
    typename y_coord_t = const_view_t::y_coord_t;
    
    Buf::Buf(x_coord_t width, y_coord_t height, std::size_t alignment=1);
    Buf::Buf(x_coord_t width, y_coord_t height, value_type fill_value, std::size_t alignment);

    x_coord_t Buf::width() const;
    y_coord_t Buf::height() const;
    
    void Buf::recreate(x_coord_t width, y_coord_t height, std::size_t alignment=1);
    void Buf::recreate(x_coord_t width, y_coord_t height, value_type fill_value, std::size_t alignment);
};
\endcode
*/
template <typename Buf>
struct RandomAccess2DBufferConcept {
    void constraints() {
        gil_function_requires<RandomAccessNDBufferConcept<Buf> >();
        typedef typename Buf::x_coord_t  x_coord_t;
        typedef typename Buf::y_coord_t  y_coord_t;
        typedef typename Buf::value_type value_t;

        gil_function_requires<MutableRandomAccess2DBufferViewConcept<typename Buf::view_t> >();

        x_coord_t w=buf.width();
        y_coord_t h=buf.height();
        value_t fill_value;
        Buf im1(w,h);
        Buf im2(w,h,1);
        Buf im3(w,h,fill_value,1);
        buf.recreate(w,h);
        buf.recreate(w,h,1);
        buf.recreate(w,h,fill_value,1);
    }
    Buf buf;
};

/// \ingroup BufferConcept
/// \brief 2-dimensional buffer whose value type models FrameValueConcept
/**
\code 
concept BufferConcept<RandomAccess2DBufferConcept Buf> {
    where MutableBufferViewConcept<view_t>;
    typename coord_t  = view_t::coord_t;
};
\endcode
*/
template <typename Buf>
struct BufferConcept {
    void constraints() {
        gil_function_requires<RandomAccess2DBufferConcept<Buf> >();
        gil_function_requires<MutableBufferViewConcept<typename Buf::view_t> >();
        typedef typename Buf::coord_t        coord_t;
        BOOST_STATIC_ASSERT(num_samples<Buf>::value == mpl::size<typename channel_space_type<Buf>::type>::value);

        BOOST_STATIC_ASSERT((is_same<coord_t, typename Buf::x_coord_t>::value));
        BOOST_STATIC_ASSERT((is_same<coord_t, typename Buf::y_coord_t>::value));
    }
    Buf buf;
};


} }  // namespace boost::gil

#endif
