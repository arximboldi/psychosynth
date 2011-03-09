/**
 *  Time-stamp:  <2011-03-08 23:50:42 raskolnikov>
 *
 *  @file        concept.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Oct 20 20:01:23 2010
 *
 *  Concepts of the sound library.
 *
 *  @todo Buffer Views are, actually, a range. We have boost
 *  range. Should we use that instead of this confusing thing?
 *
 *  @todo Add unit tests that do declare this.
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

#ifndef PSYNTH_SOUND_CONCEPT_H
#define PSYNTH_SOUND_CONCEPT_H

#include <psynth/base/concept.hpp>
#include <psynth/sound/forwards.hpp>

/**
 * @todo Why add_reference in std is not working? GCC bug?
 */

#include <type_traits>
#include <boost/type_traits.hpp>
#include <functional>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_concepts.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/size.hpp>

namespace psynth
{
namespace sound
{

/*
 *
 *       Extra iterator concepts.
 *
 */

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
	base::psynth_function_requires< ForwardIteratorIsMutableConcept<TT> >();
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
	base::psynth_function_requires< BidirectionalIteratorIsMutableConcept<TT> >();
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

template <typename ChannelSpace1, typename ChannelSpace2>
// Models ChannelSpaceConcept
struct channel_spaces_are_compatible :
    public boost::is_same<ChannelSpace1, ChannelSpace2> {};

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
        base::psynth_function_requires< boost::EqualityComparableConcept<T> >(); 
        
        typedef typename sample_traits<T>::value_type v;
        typedef typename sample_traits<T>::reference r;
        typedef typename sample_traits<T>::pointer p;
        typedef typename sample_traits<T>::const_reference cr;
        typedef typename sample_traits<T>::const_pointer cp;

        sample_traits<T>::min_value ();
        sample_traits<T>::max_value ();
	sample_traits<T>::zero_value (); // Added for sound. It is
					 // important to know which is
					 // the stable central value.
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
        base::psynth_function_requires<SampleConcept<T> >();
        base::psynth_function_requires<detail::SampleIsMutableConcept<T> >();
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
        base::psynth_function_requires<SampleConcept<T> >();
        base::psynth_function_requires<base::Regular<T> >();
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
    : public boost::is_same<typename sample_traits<T1>::value_type,
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
        base::psynth_function_requires<SampleConcept<SrcSample> >();
        base::psynth_function_requires<MutableSampleConcept<DstSample> >();
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
        base::psynth_function_requires< base::CopyConstructible<ChannelBase> >();
        base::psynth_function_requires< base::EqualityComparable<ChannelBase> >();

        typedef typename ChannelBase::layout::channel_space channel_space;
        base::psynth_function_requires<ChannelSpaceConcept<channel_space> >();

        typedef typename ChannelBase::layout_t::sample_mapping sample_mapping;
        // TODO: sample_mapping_t must be an MPL RandomAccessSequence

        static const std::size_t num_elements = size<ChannelBase>::value;

        typedef typename kth_element_type<ChannelBase,num_elements-1>::type TN; 
        typedef typename kth_element_const_reference_type<
	    ChannelBase, num_elements-1>::type CR; 

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
        base::psynth_function_requires< ChannelBaseConcept<ChannelBase> >();
        base::psynth_function_requires< base::Assignable<ChannelBase> >();
        base::psynth_function_requires< base::Swappable<ChannelBase> >();

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
        base::psynth_function_requires< MutableChannelBaseConcept<ChannelBase> >();
        base::psynth_function_requires< base::Regular<ChannelBase> >();
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
        base::psynth_function_requires< ChannelBaseConcept<ChannelBase> >();

        static const std::size_t num_elements = size<ChannelBase>::value;

        typedef typename kth_element_type<ChannelBase,0>::type T0; 
        typedef typename kth_element_type<ChannelBase,num_elements-1>::type TN; 

        BOOST_STATIC_ASSERT((boost::is_same<T0,TN>::value));
	// better than nothing
        typedef typename kth_element_const_reference_type<
	    ChannelBase,0>::type CRef0; 
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
        base::psynth_function_requires< ChannelBaseConcept<ChannelBase> >();
        base::psynth_function_requires< HomogeneousChannelBaseConcept<ChannelBase> >();
        typedef typename kth_element_reference_type<ChannelBase, 0>::type R0;
        R0 x = dynamic_at_c (cb, 0);
        dynamic_at_c(cb,0) = dynamic_at_c (cb, 0);
    }
    ChannelBase cb;
};

/**
   \ingroup ChannelBaseConcept
   \brief Homogeneous channel base that also has a default
   constructor. Refines base::Regular.
*/
/** 

\code
concept HomogeneousChannelBaseValueConcept<typename T> :
         MutableHomogeneousChannelBaseConcept<T>, base::Regular<T>
{ };
\endcode
*/

template <typename ChannelBase>
struct HomogeneousChannelBaseValueConcept
{
    void constraints() {
        base::psynth_function_requires<
	    MutableHomogeneousChannelBaseConcept<ChannelBase> >();
        base::psynth_function_requires< base::Regular<ChannelBase> >();
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
struct ChannelBasesCompatibleConcept
{
    void constraints() {
        BOOST_STATIC_ASSERT(
	    (boost::is_same<typename ChannelBase1::layout_type::channel_space, 
	     typename ChannelBase2::layout_type::channel_space>::value));
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
   iterators, locators, ranges and buffers whose value type is a frame
   
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
        typedef typename channel_space_type<P>::type channel_space;
        base::psynth_function_requires<ChannelSpaceConcept<channel_space> >();

        typedef typename sample_mapping_type<P>::type sample_mapping;
        base::psynth_function_requires<SampleMappingConcept<sample_mapping> >();

        static const bool planar = is_planar<P>::type::value;
	boost::ignore_unused_variable_warning(planar);


        // This is not part of the concept, but should still work
        static const std::size_t nc = num_samples<P>::value;
        boost::ignore_unused_variable_warning(nc);
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
    void constraints()
    {
        base::psynth_function_requires<FrameBasedConcept<P> >();

	typedef typename sample_type<P>::type sample;
        base::psynth_function_requires<SampleConcept<sample> >();        
    }
};


/**
   \brief Frame concept - A channel base whose elements are samples
   \ingroup FrameConcept
*/
/**
\code
concept FrameConcept<typename P> :
            ChannelBaseConcept<P>, FrameBasedConcept<P> {    
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
        base::psynth_function_requires<ChannelBaseConcept<P> >();
        base::psynth_function_requires<FrameBasedConcept<P> >();

        BOOST_STATIC_ASSERT((is_frame<P>::value));
        static const bool is_mutable = P::is_mutable;
	boost::ignore_unused_variable_warning(is_mutable);

        typedef typename P::value_type      value_type;
//      base::psynth_function_requires<FrameValueConcept<value_type> >();

        typedef typename P::reference       reference;
        base::psynth_function_requires<
	    FrameConcept<typename remove_const_and_reference<reference>::type> >();

        typedef typename P::const_reference const_reference;
        base::psynth_function_requires<
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
        base::psynth_function_requires<FrameConcept<P> >();
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
        base::psynth_function_requires<FrameConcept<P> >();
        base::psynth_function_requires<HomogeneousChannelBaseConcept<P> >();
        base::psynth_function_requires<HomogeneousFrameBasedConcept<P> >();
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
        base::psynth_function_requires<HomogeneousFrameConcept<P> >();
        base::psynth_function_requires<MutableHomogeneousChannelBaseConcept<P> >();
        p[0] = p[0];
    }
    P p;
};

/**
   \brief Frame concept that is a base::Regular type
   \ingroup FrameConcept
*/
/**
\code
concept FrameValueConcept<FrameConcept P> : base::Regular<P> {
    where SameType<value_type,P>;
};    
\endcode
*/
template <typename P>
struct FrameValueConcept
{
    void constraints() {
        base::psynth_function_requires<FrameConcept<P> >();
        base::psynth_function_requires<base::Regular<P> >();
    }
};

/**
   \brief Homogeneous frame concept that is a base::Regular type
   \ingroup FrameConcept
*/
/**
\code
concept HomogeneousFrameValueConcept<HomogeneousFrameConcept P> : base::Regular<P> {
    where SameType<value_type,P>;
}; 
\endcode
*/
template <typename P>
struct HomogeneousFrameValueConcept
{
    void constraints() {
        base::psynth_function_requires<HomogeneousFrameConcept<P> >();
        base::psynth_function_requires<base::Regular<P> >();
        BOOST_STATIC_ASSERT((boost::is_same<P, typename P::value_type>::value));
    }
};

namespace detail
{
    template <typename P1, typename P2, int K>
    struct samples_are_pairwise_compatible : public 
        boost::mpl::and_<
	samples_are_pairwise_compatible<P1,P2,K-1>,
	samples_are_compatible<
	    typename kth_semantic_element_reference_type<P1,K>::type,
	    typename kth_semantic_element_reference_type<P2,K>::type> >
    {};
                                                 
    template <typename P1, typename P2>
    struct samples_are_pairwise_compatible<P1, P2, -1> :
	public boost::mpl::true_ {};
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
    : public boost::mpl::and_<typename channel_spaces_are_compatible<
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
        base::psynth_function_requires<FrameConcept<SrcP> >();
        base::psynth_function_requires<MutableFrameConcept<DstP> >();
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
  : DefaultConstructibleConcept<D>, base::CopyConstructibleConcept<D>,
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
        base::psynth_function_requires< boost::UnaryFunctionConcept<D, 
            typename remove_const_and_reference<typename D::result_type>::type, 
            typename D::argument_type> >();
        base::psynth_function_requires< boost::DefaultConstructibleConcept<D> >();
        base::psynth_function_requires< boost::CopyConstructibleConcept<D> >();
        base::psynth_function_requires< boost::AssignableConcept<D> >();

        base::psynth_function_requires<FrameConcept<
	    typename remove_const_and_reference<
		typename D::result_type>::type> >();

        typedef typename D::const_t const_t;
        base::psynth_function_requires<FrameDereferenceAdaptorConcept<const_t> >();
        typedef typename D::value_type value_type;
        base::psynth_function_requires<FrameValueConcept<value_type> >();
        typedef typename D::reference reference;
	// == FrameConcept (if you remove const and reference)
        typedef typename D::const_reference const_reference;
	// == FrameConcept (if you remove const and reference)

        const bool is_mutable = D::is_mutable;
	boost::ignore_unused_variable_warning (is_mutable);
    }
    D d;
};

template <typename P>
struct FrameDereferenceAdaptorArchetype : public std::unary_function<P, P>
{
    typedef FrameDereferenceAdaptorArchetype const_t;
    typedef typename boost::remove_reference<P>::type value_type;
    typedef typename boost::add_reference<P>::type reference;
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
   \brief Concept for iterators, locators and ranges that can define a
   type just like the given iterator/locator/range, except it supports
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
struct HasDynamicStepTypeConcept
{   
    void constraints () {
        typedef typename dynamic_step_type<T>::type type;
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
        base::psynth_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<Iterator> >();
        base::psynth_function_requires<FrameBasedConcept<Iterator> >();
        
        typedef typename std::iterator_traits<Iterator>::value_type value_type;
        base::psynth_function_requires<FrameValueConcept<value_type> >();
 
        typedef typename const_iterator_type<Iterator>::type const_t;
        static const bool is_mut = iterator_is_mutable<Iterator>::type::value;
	boost::ignore_unused_variable_warning (is_mut);

        const_t const_it(it);
	boost::ignore_unused_variable_warning(const_it);
        // immutable iterator must be constructible from (possibly
	// mutable) iterator

        check_base (typename is_iterator_adaptor<Iterator>::type ());
    }
    
    void check_base(boost::mpl::false_) {}

    void check_base(boost::mpl::true_)
    {
        typedef typename iterator_adaptor_get_base<Iterator>::type base_t;
        base::psynth_function_requires<FrameIteratorConcept<base_t> >();
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
            base::psynth_function_requires<
		detail::RandomAccessIteratorIsMutableConcept<Iterator> >();
            typedef typename boost::remove_reference<
		typename std::iterator_traits<Iterator>::reference>::type ref;
            typedef typename element_type<ref>::type sample_t;
            base::psynth_function_requires<detail::SampleIsMutableConcept<sample_t> >();
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
        base::psynth_function_requires<FrameIteratorConcept<Iterator> >();
        base::psynth_function_requires<
	    detail::FrameIteratorIsMutableConcept<Iterator> >();
    }
};

namespace detail
{

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
	std::ptrdiff_t bs=memunit_step(it);
	boost::ignore_unused_variable_warning (bs);
	it=memunit_advanced(it,3);
	std::ptrdiff_t bd=memunit_distance(it,it);
	boost::ignore_unused_variable_warning(bd);
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
        base::psynth_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<Iterator> >();
        base::psynth_function_requires<
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
        base::psynth_function_requires<
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
        base::psynth_function_requires<StepIteratorConcept<Iterator> >();
        base::psynth_function_requires<
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
 - \p is_iterator_adaptor<Iterator>:             Returns \p boost::mpl::true_
 - \p iterator_adaptor_get_base<Iterator>:       Returns the base iterator type
 - \p iterator_adaptor_rebind<Iterator,NewBase>: Replaces the base
                                                 iterator with the new one
   
   The adaptee can be obtained from the iterator via the "base()"
   method.
   
\code
concept IteratorAdaptorConcept<boost_concepts::ForwardTraversalConcept Iterator> {
    where SameType<is_iterator_adaptor<Iterator>::type, boost::mpl::true_>;

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
        base::psynth_function_requires<
	    boost_concepts::ForwardTraversalConcept<Iterator> >();

        typedef typename iterator_adaptor_get_base<Iterator>::type base_t;
        base::psynth_function_requires<
	    boost_concepts::ForwardTraversalConcept<base_t> >();

        BOOST_STATIC_ASSERT(is_iterator_adaptor<Iterator>::value);
        typedef typename iterator_adaptor_rebind<Iterator, void*>::type rebind_t;

        base_t base=it.base();  boost::ignore_unused_variable_warning(base);
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
        base::psynth_function_requires<IteratorAdaptorConcept<Iterator> >();
        base::psynth_function_requires<
	    detail::ForwardIteratorIsMutableConcept<Iterator> >();
    }
};


/*
 *
 *             BUFFER RANGE CONCEPTS
 *
 */

/**
   \defgroup BufferRangeConcept BufferRangeConcept
   \ingroup BufferRangeConcept
   \brief 1-dimensional range

   \defgroup FrameBufferRangeConcept BufferRangeConcept
   \ingroup BufferRangeConcept
   \brief 1-dimensional range over frame data
   
   \ingroup BufferRangeConcept
   \brief N-dimensional range over immutable values
*/

 /**
\code
concept RandomAccessBufferRangeConcept<base::Regular Range> {
    typename value_type;
    typename reference;       // result of dereferencing
    typename difference_type;
    // result of operator-(iterator,iterator) (1-dimensional!)
    typename const_t;  where RandomAccessNDBufferRangeConcept<Range>;
    // same as Range, but over immutable values
    typename point_t;  where PointNDConcept<point_t>; // N-dimensional point
    typename locator;  where RandomAccessNDLocatorConcept<locator>;
    // N-dimensional locator.
    typename iterator; where RandomAccessTraversalConcept<iterator>;
    // 1-dimensional iterator over all values
    typename reverse_iterator;
    where RandomAccessTraversalConcept<reverse_iterator>; 
    typename size_type;       // the return value of size()

    // Equivalent to RandomAccessNDLocatorConcept::axis
    template <size_t D> struct axis {
        typename coord_t = point_t::axis<D>::coord_t;
        typename iterator; where
    RandomAccessTraversalConcept<iterator>;
    // iterator along D-th axis.
        where SameType<coord_t, iterator::difference_type>;
        where SameType<iterator::value_type,value_type>;
    };

    // Defines the type of a range similar to this type, except it
    // invokes Deref upon dereferencing
    template <FrameDereferenceAdaptorConcept Deref> struct add_deref {
        typename type;        where RandomAccessNDBufferRangeConcept<type>;
        static type make(const Range& v, const Deref& deref);
    };

    static const size_t num_dimensions = point_t::num_dimensions;
    
    // Create from a locator at the top-left corner and dimensions
    Range::Range(const locator&, const point_type&);
    
    size_type        Range::size()       const; // total number of elements
    reference        operator[](Range, const difference_type&) const;
    // 1-dimensional reference
    iterator         Range::begin()      const;
    iterator         Range::end()        const;
    reverse_iterator Range::rbegin()     const;
    reverse_iterator Range::rend()       const;
    iterator         Range::at(const point_t&);
    point_t          Range::dimensions() const; // number of elements along each dimension
    bool             Range::is_1d_traversable() const;   // can an iterator over the first dimension visit each value? I.e. are there gaps between values?

    // iterator along a given dimension starting at a given point
    template <size_t D> Range::axis<D>::iterator Range::axis_iterator(const point_t&) const;

    reference operator()(Range,const point_t&) const;
};
\endcode
*/
template <typename Range>
struct RandomAccessBufferRangeConcept
{
    void constraints ()
    {
        base::psynth_function_requires< base::Regular<Range> >();

        typedef typename Range::value_type       value_type;
        typedef typename Range::reference        reference;
	// result of dereferencing
        typedef typename Range::difference_type  difference_type;
	// result of operator-(1d_iterator,1d_iterator)

	typedef typename Range::iterator         iterator;
        typedef typename Range::reverse_iterator reverse_iterator;
        typedef typename Range::size_type        size_type;
            
	base::psynth_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<iterator> >();
        base::psynth_function_requires<
	    boost_concepts::RandomAccessTraversalConcept<reverse_iterator> >();

        iterator it;
        reverse_iterator rit;
        difference_type d;
	size_type sz;
	detail::initialize_it (d); boost::ignore_unused_variable_warning(d);

        Range (sz, it); // range must be constructible from a locator and a point

	// TODO: Should we minimize this interface to match Boost.Range?
        sz = range.size ();  boost::ignore_unused_variable_warning(sz);
        it = range.frames ();
        
        it  = range.begin ();
        it  = range.end ();
        rit = range.rbegin ();
        rit = range.rend ();

        reference r1=range[d]; boost::ignore_unused_variable_warning(r1);
	// 1D access 
        
        typedef FrameDereferenceAdaptorArchetype<typename Range::value_type>
	    deref_t;
        typedef typename Range::template add_deref<deref_t>::type dtype;
    }
    Range range;
};


/**
   \ingroup FrameBufferRangeConcept
   \brief GIL's 2-dimensional range over immutable GIL frames
*/
/**
\code
concept BufferRangeConcept<RandomAccess2DBufferRangeConcept Range> {
    where FrameValueConcept<value_type>;
    where FrameIteratorConcept<x_iterator>;        
    where FrameIteratorConcept<y_iterator>;
    where x_coord_t == y_coord_t;
    
    typename coord_t = x_coord_t;

    std::size_t Range::num_samples() const;
};
\endcode
*/
template <typename Range>
struct BufferRangeConcept {
    void constraints() {
        base::psynth_function_requires<RandomAccessBufferRangeConcept<Range> >();

        std::size_t num_chan = range.num_samples ();
	boost::ignore_unused_variable_warning (num_chan);
    }
    Range range;
};


namespace detail {

template <typename Range>
// Preconditions: Range Models RandomAccessNDBufferRangeConcept
struct RandomAccessBufferRangeIsMutableConcept
{
    void constraints() {
	base::psynth_function_requires<
	    detail::RandomAccessIteratorIsMutableConcept<
		typename Range::iterator> >();
	base::psynth_function_requires<
	    detail::RandomAccessIteratorIsMutableConcept<
		typename Range::reverse_iterator> >();
	
	typename Range::difference_type diff;
	initialize_it(diff); boost::ignore_unused_variable_warning(diff);
	typename Range::value_type v; initialize_it(v);
	
	range[diff]=v;
    }
    Range range;
};

template <typename Range>    // preconditions: Range Models BufferRangeConcept
struct FrameBufferRangeIsMutableConcept
{
    void constraints() {        
	base::psynth_function_requires<
	    detail::RandomAccessBufferRangeIsMutableConcept<Range> >();
    }
};

} /* namespace detail */

/**
   \ingroup BufferRangeNDConcept
   \brief N-dimensional range over mutable values
*/
/**
   \code
concept MutableRandomAccessNDBufferRangeConcept<
   RandomAccessNDBufferRangeConcept Range> {
    where Mutable<reference>;
};
\endcode
*/
template <typename Range>
struct MutableRandomAccessBufferRangeConcept
{
    void constraints() {
        base::psynth_function_requires<RandomAccessBufferRangeConcept<Range> >();
        base::psynth_function_requires<
	    detail::RandomAccessBufferRangeIsMutableConcept<Range> >();
    }
};

/**
   \ingroup FrameBufferRangeConcept
   \brief GIL's 2-dimensional range over mutable GIL frames
*/
/**
   \code
   concept MutableBufferRangeConcept<BufferRangeConcept Range> :
         MutableRandomAccess2DBufferRangeConcept<Range> {};
\endcode
*/
template <typename Range>
struct MutableBufferRangeConcept
{
    void constraints() {
        base::psynth_function_requires<BufferRangeConcept<Range> >();
        base::psynth_function_requires<
	    detail::FrameBufferRangeIsMutableConcept<Range> >();
    }
};

/**
   \brief Returns whether two ranges are compatible
   
   Ranges are compatible if their frames are compatible. Compatible
   ranges can be assigned and copy constructed from one another.
*/
template <typename V1, typename V2>  // Model BufferRangeConcept
struct ranges_are_compatible :
    public frames_are_compatible<typename V1::value_type,
				 typename V2::value_type> {};


/**
   \brief Ranges are compatible if they have the same channel spaces
   and compatible sample values. Constness and layout are not
   important for compatibility
   
   \ingroup BufferRangeConcept
*/
/**
\code
concept RangesCompatibleConcept<BufferRangeConcept V1, BufferRangeConcept V2> {
    where FramesCompatibleConcept<V1::value_type, P2::value_type>;
};
\endcode
*/
template <typename V1, typename V2>
struct RangesCompatibleConcept
{
    void constraints () {
        BOOST_STATIC_ASSERT((ranges_are_compatible<V1,V2>::value));
    }
};


/*
 *
 *            BUFFER CONCEPTS
 *
 */

/**
   \ingroup BufferConcept
   \brief N-dimensional container of values
*/
/**
\code
concept RandomAccessBufferConcept<typename Buf> : base::Regular<Buf> {
    typename range_t; where MutableRandomAccessBufferRangeConcept<range_t>;
    typename const_range_t = range_t::const_t;
    typename point_t      = range_t::point_t;
    typename value_type   = range_t::value_type;
    typename allocator_type;

    Buf::Buf(point_t dims, std::size_t alignment=1);
    Buf::Buf(point_t dims, value_type fill_value, std::size_t alignment);
    
    void Buf::recreate(point_t new_dims, std::size_t alignment=1);
    void Buf::recreate(point_t new_dims, value_type fill_value, std::size_t alignment);

    const point_t&        Buf::dimensions() const;
    const const_range_t&   const_range(const Buf&);
    const range_t&         range(Buf&);
};
\endcode
*/
template <typename Buf>
struct RandomAccessBufferConcept
{
    void constraints() {
        base::psynth_function_requires<base::Regular<Buf> >();

        typedef typename Buf::range       range_t;
        base::psynth_function_requires<
	    MutableRandomAccessBufferRangeConcept<range_t> >();

        typedef typename Buf::const_range const_range_t;
        typedef typename Buf::value_type   frame_t;

        const_range_t cv = const_range(buf);
	boost::ignore_unused_variable_warning(cv);
        range_t       v  = range(buf);
	boost::ignore_unused_variable_warning(v);

        frame_t fill_value;
        typename Buf::size_type pt = buf.size ();
        Buf im1 (pt);
        Buf im2 (pt,1);
        Buf im3 (pt, fill_value, 1);
        buf.recreate(pt);
        buf.recreate(pt,1);
        buf.recreate(pt,fill_value,1);
    }
    Buf buf;
};


/**
   \ingroup BufferConcept
   \brief 2-dimensional buffer whose value type models
   FrameValueConcept
*/
/**
\code 
concept BufferConcept<RandomAccess2DBufferConcept Buf> {
    where MutableBufferRangeConcept<range_t>;
    typename coord_t  = range_t::coord_t;
};
\endcode
*/
template <typename Buf>
struct BufferConcept
{
    void constraints() {
        base::psynth_function_requires<RandomAccessBufferConcept<Buf> >();
        base::psynth_function_requires<MutableBufferRangeConcept<
	    typename Buf::range> >();
	
        BOOST_STATIC_ASSERT(num_samples<Buf>::value ==
			    boost::mpl::size<
				typename channel_space_type<Buf>::type>::value);
    }
    Buf buf;
};


} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_CONCEPT */
