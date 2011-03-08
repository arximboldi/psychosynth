/**
 *  Time-stamp:  <2011-03-08 18:59:19 raskolnikov>
 *
 *  @file        buffer_range_factory.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Oct 29 11:55:36 2010
 *
 *  Range factories.  Methods for creating shallow image ranges from raw
 *  pixel data or from other image ranges - flipping horizontally or
 *  vertically, axis-aligned rotation, a subimage, sub_sampled or n-th
 *  channel image range. Derived image ranges are shallow copies and are
 *  fast to construct.
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

#ifndef PSYNTH_SOUND_BUFFER_RANGE_FACTORY_HPP
#define PSYNTH_SOUND_BUFFER_RANGE_FACTORY_HPP

#include <cassert>
#include <cstddef>

#include <psynth/base/compat.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/mono.hpp>
#include <psynth/sound/channel_convert.hpp>

/**
   \defgroup BufferRangeConstructors Buffer Range From Raw Data
   \ingroup BufferRangeAlgorithm
   \brief Methods for constructing buffer ranges from raw data and for
   getting raw data from ranges
*/

/**
   \defgroup BufferRangeTransformations Buffer Range Transformations
   \ingroup BufferRangeAlgorithm
   \brief Methods for constructing one buffer range from another
*/

namespace psynth
{
namespace sound
{
struct default_channel_converter;

template <typename T> struct dynamic_step_type;
template <typename T> struct transposed_type;

/**
   \ingroup BufferRangeConstructors
   \brief Constructing buffer ranges from raw interleaved frame data
*/
template <typename Iterator>
typename type_from_iterator<Iterator>::range
interleaved_range (std::size_t size, Iterator frames)
{
    typedef typename type_from_iterator<Iterator>::range RRange;
    return RRange (size, typename RRange::iterator (frames));
}

/**
   interleaved_range_get_raw_data, planar_range_get_raw_data - return
   pointers to the raw data (the samples) of a basic homogeneous range.
*/

namespace detail
{

template <typename Range, bool IsMutable>
struct sample_pointer_type_impl;

template <typename Range> struct sample_pointer_type_impl<Range, true>
{
    typedef typename sample_type<Range>::type* type;
};

template <typename Range> struct sample_pointer_type_impl<Range, false>
{
    typedef const typename sample_type<Range>::type* type;
};

template <typename Range> struct sample_pointer_type
    : public sample_pointer_type_impl<Range, range_is_mutable<Range>::value> {};

} /* namespace detail */


/**
   \ingroup BufferRangeConstructors
   \brief Returns C pointer to the the samples of an interleaved
   homogeneous range.
*/
template <typename HomogeneousRange>
typename detail::sample_pointer_type<HomogeneousRange>::type
interleaved_range_get_raw_data(const HomogeneousRange& range)
{
    BOOST_STATIC_ASSERT((!is_planar<HomogeneousRange>::value &&
			 range_is_basic<HomogeneousRange>::value));
    BOOST_STATIC_ASSERT((boost::is_pointer<
                         typename HomogeneousRange::iterator>::value));

    return &psynth::sound::at_c<0>(range [0]);
}

/**
   \ingroup BufferRangeConstructors
   \brief Returns C pointer to the the samples of a given channel
   plane of a planar homogeneous range.
*/
template <typename HomogeneousRange>
typename detail::sample_pointer_type<HomogeneousRange>::type
planar_range_get_raw_data(const HomogeneousRange& range, int plane_index)
{
    BOOST_STATIC_ASSERT((is_planar<HomogeneousRange>::value &&
			 range_is_basic<HomogeneousRange>::value));
    return dynamic_at_c (range.begin (), plane_index);
}


/**
   \defgroup BufferRangeTransformationsChannelConvert channel_converted_range
   \ingroup BufferRangeTransformations
   \brief Channel converted range of another range

   \ingroup BufferRangeTransformationsChannelConvert FrameDereferenceAdaptorModel
   \brief Function object that given a source frame, returns it converted to a given channel space and sample depth. Models: FrameDereferenceAdaptorConcept
   
   Useful in constructing a channel converted range over a given buffer range
*/

template <typename SrcConstRefP, typename DstP,
	  typename CC=default_channel_converter>
// const_reference to the source frame and destination frame value
class channel_convert_deref_fn :
        public deref_base<channel_convert_deref_fn<SrcConstRefP, DstP, CC>,
                          DstP, DstP, const DstP&, SrcConstRefP, DstP, false>
{
public:
    channel_convert_deref_fn () {}
    channel_convert_deref_fn (CC cc_in) : _cc (cc_in) {}

    DstP operator () (SrcConstRefP srcP) const
    {
        DstP dstP;
        _cc (srcP, dstP);
        return dstP;
    }
    
private:
    CC _cc; // channel-converter
};


namespace detail
{

// Add channel converter upon dereferencing
template <typename SrcRange, typename CC, typename DstP, typename SrcP>
struct channel_converted_range_type_impl
{
private:
    typedef channel_convert_deref_fn<
    typename SrcRange::const_type::reference,DstP,CC> deref_t;
    typedef typename SrcRange::template add_deref<deref_t> add_ref_t;
    
public:
    typedef typename add_ref_t::type type;
    static type make (const SrcRange& sv,CC cc)
    {
	return add_ref_t::make (sv, deref_t (cc));
    }
};

/* If the Src range has the same frame type as the target, there is
   no need for channel conversion */
template <typename SrcRange, typename CC, typename DstP>
struct channel_converted_range_type_impl<SrcRange,CC,DstP,DstP>
{
    typedef SrcRange type;
    static type make (const SrcRange& sv, CC)
    { return sv; }
};

} /* namespace detail */


/**
   \brief Returns the type of a range that does channel conversion upon
   dereferencing its frames
   
   \ingroup BufferRangeTransformationsChannelConvert
*/
template <typename SrcRange, typename DstP, typename CC=default_channel_converter>
struct channel_converted_range_type :
        public detail::channel_converted_range_type_impl<SrcRange,
                                                         CC,
                                                         DstP,
                                                         typename SrcRange::value_type>
{
    PSYNTH_CLASS_REQUIRE(DstP, psynth::sound, MutableFrameConcept)
    // Why does it have to be mutable???
};


/**
   \ingroup BufferRangeTransformationsChannelConvert
   \brief range of a different channel space with a user defined
   channel-converter
*/
template <typename DstP, typename Range, typename CC>
inline typename channel_converted_range_type<Range, DstP, CC>::type
channel_converted_range (const Range& src, CC cc)
{
    return channel_converted_range_type<Range,DstP,CC>::make(src,cc);
}

/**
   \ingroup BufferRangeTransformationsChannelConvert
   \brief overload of generic channel_converted_range with the default
   channel-converter
*/
template <typename DstP, typename Range>
inline typename channel_converted_range_type<Range,DstP>::type
channel_converted_range (const Range& src)
{
    return channel_converted_range<DstP>(src, default_channel_converter());
}


/**
   \defgroup BufferRangeTransformationsFlipLR flipped_left_right_range
   \ingroup BufferRangeTransformations
   \brief range of a range flipped left-to-right

   \ingroup BufferRangeTransformationsFlipLR
*/
template <typename Range> 
inline typename dynamic_step_type<Range>::type flipped_range (const Range& src)
{
    typedef typename dynamic_step_type<Range>::type RRange;
    return RRange (src.size (), make_step_iterator (src.end () - 1, -1));
}

/**
   \defgroup BufferRangeTransformationsSubbuffer subbuffer_range
   \ingroup BufferRangeTransformations
   \brief range of an axis-aligned rectangular area within an buffer_range

   \ingroup BufferRangeTransformationsSubbuffer
*/
template <typename Range>
inline typename buffer_range_type<Range>::type
sub_range (const Range& src, size_t start, size_t size)
{
    typedef typename buffer_range_type<Range>::type RRange;
    return RRange (size, src.at (start));
}

/**
   \defgroup BufferRangeTransformationsSubbuffer subbuffer_range
   \ingroup BufferRangeTransformations
   \brief range of an axis-aligned rectangular area within an buffer_range

   \ingroup BufferRangeTransformationsSubbuffer
   
   @todo This is an overload for buffer to avoid the container vs
   range constness problem. Maybe we should step back in front of this
   constness nightmare?
*/
template <typename F, bool P, typename A>
class buffer;

template <typename F, bool P, typename A>
inline typename buffer_range_type<buffer<F,P,A> >::type
sub_range (buffer<F,P,A>& src, size_t start, size_t size)
{
    typedef typename buffer_range_type<buffer<F,P,A> >::type RRange;
    return RRange (size, src.at (start));
}

/**
   \defgroup BufferRangeTransformationsSubbuffer subbuffer_range
   \ingroup BufferRangeTransformations
   \brief range of an axis-aligned rectangular area within an buffer_range

   \ingroup BufferRangeTransformationsSubbuffer
   
   @todo This is a non-const version. Non const versions of
   range factories are needed because buffers are now ranges. Keep
   working on this.
*/

#ifdef PSYNTH_BUFFER_MODEL_RANGE

template <typename Range>
inline typename buffer_range_type<Range>::type
sub_range (Range& src,
           const typename Range::size_type start,
           const typename Range::size_type size)
{
    typedef typename buffer_range_type<Range>::type RRange;
    return RRange (size, src.at (start));
}

#endif /* PSYNTH_BUFFER_MODEL_RANGE */

/**
   \defgroup BufferRangeTransformationsSub_Sampled sub_sampled_range
   \ingroup BufferRangeTransformations
   \brief range of a sub_sampled version of an buffer_range, stepping
   over a number of samples in X and number of rows in Y

   \ingroup BufferRangeTransformationsSub_Sampled
*/
template <typename Range> 
inline typename dynamic_step_type<Range>::type
sub_sampled_range (const Range& src, typename Range::size_type step)
{
    assert (step > 0);
    typedef typename dynamic_step_type<Range>::type RRange;
    return RRange ((src.size () + (step - 1)) / step,
                   make_step_iterator (src.begin (), step));
}


/**
   \defgroup BufferRangeTransformationsNthSample nth_sample_range
   \ingroup BufferRangeTransformations
   \brief single-sample (grayscale) range of the N-th sample of a given
   buffer_range
*/

namespace detail
{
template <typename Range, bool AreSamplesTogether>
struct nth_sample_range_basic_impl;

/**
   nth_sample_range when the samples are not adjacent in memory. This
   can happen for multi-sample interleaved buffers or buffers with a
   step
*/
template <typename Range>
struct nth_sample_range_basic_impl<Range, false>
{
    typedef typename range_type<typename sample_type<Range>::type,
                                mono_layout, false, true,
                                range_is_mutable<Range>::value>::type type;

    static type make (const Range& src, int n)
    {
	typedef typename type::iterator iterator;
	typedef typename iterator_adaptor_get_base <iterator>::type iterator_base;
	
	return type (src.size (),
		     iterator (iterator_base (& (src [0] [n])),
			       src.frame_size ()));
    }
};

/*
  nth_sample_range when the samples are together in memory (true for
  simple grayscale or planar buffers)
*/
template <typename Range>
struct nth_sample_range_basic_impl<Range, true>
{
    typedef typename range_type<typename sample_type<Range>::type,
                                mono_layout, false, false,
                                range_is_mutable<Range>::value>::type type;

    static type make (const Range& src, int n)
    {
	typedef typename type::iterator iterator;
	return interleaved_range (src.size (), (iterator) &(src [0][n]));
    }
};

template <typename Range, bool IsBasic>
struct nth_sample_range_impl;

// For basic (memory-based) ranges dispatch to nth_sample_range_basic_impl
template <typename Range> struct nth_sample_range_impl<Range,true>
{
private:
    typedef typename Range::iterator src_iterator;

    // Determines whether the samples of a given frame iterator are
    // adjacent in memory.  Planar and grayscale iterators have
    // samples adjacent in memory, whereas multi-sample interleaved
    // and iterators with non-fundamental step do not.
    BOOST_STATIC_CONSTANT(bool, adjacent=
			  !iterator_is_step<src_iterator>::value &&
			  (is_planar<src_iterator>::value ||
			   num_samples<Range>::value==1));
public:
    typedef typename nth_sample_range_basic_impl <Range, adjacent>::type type;

    static type make (const Range& src, int n)
    {
	return nth_sample_range_basic_impl <Range,adjacent>::make (src,n);
    }
};

/**
   \brief Function object that returns a grayscale reference of the
   N-th sample of a given reference. Models:
   FrameDereferenceAdaptorConcept.
   
   \ingroup FrameDereferenceAdaptorModel

   If the input is a frame value or constant reference, the function
   object is immutable. Otherwise it is mutable (and returns non-const
   reference to the n-th sample)
*/

template <typename SrcP>
// SrcP is a reference to FrameConcept (could be frame value or
// const/non-const reference) Examples: frame<T,L>, frame<T,L>&, const
// frame<T,L>&, planar_frame_reference<T&,L>,
// planar_frame_reference<const T&,L>
struct nth_sample_deref_fn
{
    BOOST_STATIC_CONSTANT(bool, is_mutable=frame_is_reference<SrcP>::value &&
			  frame_reference_is_mutable<SrcP>::value);

private:
    typedef typename boost::remove_reference<SrcP>::type       src_frame_t;
    typedef typename sample_type<src_frame_t>::type     sample_t;
    typedef typename src_frame_t::const_reference       const_ref_t;
    typedef typename frame_reference_type<
	sample_t, mono_layout, false, is_mutable>::type ref_t;

    int _n;        // the sample to use
    
public:
    typedef nth_sample_deref_fn<const_ref_t>       const_type;
    typedef typename frame_value_type<
	sample_t, mono_layout>::type               value_type;
    typedef typename frame_reference_type<
	sample_t,mono_layout,false,false>::type    const_reference;
    typedef SrcP                                   argument_type;
    typedef typename boost::mpl::if_c<
	is_mutable, ref_t, value_type>::type       reference;
    typedef reference                              result_type;

    nth_sample_deref_fn (int n = 0)
	: _n(n)
    {}

    template <typename P>
    nth_sample_deref_fn (const nth_sample_deref_fn<P>& d)
	: _n(d._n) {}

    result_type operator () (argument_type src_f) const { 
	return result_type (src_f[_n]); 
    }
};

template <typename Range>
struct nth_sample_range_impl<Range,false>
{
private:
    typedef nth_sample_deref_fn<typename Range::reference> deref_t;
    typedef typename Range::template add_deref<deref_t>    AD;
    
public:
    typedef typename AD::type type;
    static type make(const Range& src, int n)
    {
	return AD::make(src, deref_t(n));
    }
};

} /* namespace detail */

/**
   \brief Given a source buffer range type Range, returns the type of an
   buffer range over a single sample of Range
   \ingroup BufferRangeTransformationsNthSample
   
   If the samples in the source range are adjacent in memory (such as
   planar non-step range or single-sample range) then the return range is
   a single-sample non-step range.  If the samples are non-adjacent
   (interleaved and/or step range) then the return range is a
   single-sample step range.
*/
template <typename Range>
struct nth_sample_range_type
{
private:
    PSYNTH_CLASS_REQUIRE(Range, psynth::sound, BufferRangeConcept)
    typedef detail::nth_sample_range_impl<Range, range_is_basic<Range>::value> VB;

public:
    typedef typename VB::type type;
    static type make(const Range& src, int n) { return VB::make(src,n); }
};


/**
   \ingroup BufferRangeTransformationsNthSample
*/
template <typename Range>
typename nth_sample_range_type<Range>::type
nth_sample_range (const Range& src, int n)
{
    return nth_sample_range_type<Range>::make (src,n);
}


/**
   \defgroup BufferRangeTransformationsKthSample kth_sample_range
   \ingroup BufferRangeTransformations
   \brief single-sample (grayscale) range of the K-th sample of a given
   buffer_range. The sample index is a template parameter
*/

namespace detail
{

template <int K, typename Range, bool AreSamplesTogether>
struct kth_sample_range_basic_impl;

/*
  kth_sample_range when the samples are not adjacent in memory. This
  can happen for multi-sample interleaved buffers or buffers with a
  step
*/
template <int K, typename Range>
struct kth_sample_range_basic_impl <K, Range, false>
{
private:
    typedef typename kth_element_type<typename Range::value_type,K>::type sample_t;

public:
    typedef typename range_type<sample_t, mono_layout, false, true,
                                range_is_mutable<Range>::value>::type type;

    static type make(const Range& src)
    {
	typedef typename type::iterator iterator;
	typedef typename iterator_adaptor_get_base<iterator>::type iterator_base;
	return type(src.size (),
		    iterator (iterator_base (&sound::at_c<K> (src [0])),
			      src.frame_size ()));
    }
};

/*
  kth_sample_range when the samples are together in memory (true for
  simple grayscale or planar buffers)
*/
template <int K, typename Range>
struct kth_sample_range_basic_impl <K, Range, true>
{
private:
    typedef typename kth_element_type<typename Range::value_type, K>::type sample_t;

public:
    typedef typename range_type<
    sample_t, mono_layout, false, false, range_is_mutable<Range>::value>::type type;

    static type make (const Range& src)
    {
	typedef typename type::iterator iterator;
	return interleaved_range (src.size (), (iterator) &sound::at_c<K> (src[0]));
    }
};

template <int K, typename Range, bool IsBasic>
struct kth_sample_range_impl;

/* For basic (memory-based) ranges dispatch to __kth_sample_range_basic */
template <int K, typename Range>
struct kth_sample_range_impl <K, Range, true>
{
private:
    typedef typename Range::iterator src_iterator;

    // Determines whether the samples of a given frame iterator are
    // adjacent in memory.  Planar and grayscale iterators have
    // samples adjacent in memory, whereas multi-sample interleaved
    // and iterators with non-fundamental step do not.
    BOOST_STATIC_CONSTANT(bool, adjacent=
			  !iterator_is_step<src_iterator>::value &&
			  (is_planar<src_iterator>::value ||
			   num_samples<Range>::value==1));

public:
    typedef typename kth_sample_range_basic_impl <K, Range, adjacent>::type type;

    static type make (const Range& src)
    {
	return kth_sample_range_basic_impl <K, Range, adjacent>::make (src);
    }
};

/**
   \brief Function object that returns a grayscale reference of the
   K-th sample (specified as a template parameter) of a given
   reference. Models: FrameDereferenceAdaptorConcept.
   
   \ingroup FrameDereferenceAdaptorModel

   If the input is a frame value or constant reference, the function
   object is immutable. Otherwise it is mutable (and returns non-const
   reference to the k-th sample)
*/
template <int K, typename SrcP>
// SrcP is a reference to FrameConcept (could be frame value or
// const/non-const reference) Examples: frame<T,L>, frame<T,L>&, const
// frame<T,L>&, planar_frame_reference<T&,L>,
// planar_frame_reference<const T&,L>
struct kth_sample_deref_fn
{
    BOOST_STATIC_CONSTANT(bool, is_mutable = frame_is_reference<SrcP>::value &&
			  frame_reference_is_mutable<SrcP>::value);

private:
    typedef typename boost::remove_reference<SrcP>::type src_frame_t;
    typedef typename kth_element_type<src_frame_t, K>::type sample_t;
    typedef typename src_frame_t::const_reference const_ref_t;
    typedef typename frame_reference_type<
        sample_t, mono_layout, false, is_mutable>::type ref_t;

public:
    typedef kth_sample_deref_fn<K,const_ref_t>    const_type;
    typedef typename frame_value_type<
	sample_t, mono_layout>::type              value_type;
    typedef typename frame_reference_type<
	sample_t, mono_layout,false,false>::type  const_reference;
    typedef SrcP                                  argument_type;
    typedef typename boost::mpl::if_c<
        is_mutable, ref_t, value_type>::type  reference;
    typedef reference                             result_type;
    
    kth_sample_deref_fn () {}
    template <typename P>
    kth_sample_deref_fn (const kth_sample_deref_fn<K, P>&) {}
    
    result_type operator () (argument_type srcframe) const
    { 
	return result_type (sound::at_c<K>(srcframe));
    }
};

template <int K, typename Range>
struct kth_sample_range_impl<K,Range,false>
{
private:
    typedef kth_sample_deref_fn<K,typename Range::reference> deref_t;
    typedef typename Range::template add_deref<deref_t>   AD;

public:
    typedef typename AD::type type;
    static type make (const Range& src) { return AD::make (src, deref_t ()); }
};

} /* namespace detail */

/**
   \brief Given a source buffer range type Range, returns the type of an
   buffer range over a given sample of Range.
   
   \ingroup BufferRangeTransformationsKthSample
   
   If the samples in the source range are adjacent in memory (such as
   planar non-step range or single-sample range) then the return range is
   a single-sample non-step range.  If the samples are non-adjacent
   (interleaved and/or step range) then the return range is a
   single-sample step range.
*/
template <int K, typename Range>
struct kth_sample_range_type
{
private:
    PSYNTH_CLASS_REQUIRE(Range, psynth::sound, BufferRangeConcept)
    typedef detail::kth_sample_range_impl<K, Range, range_is_basic<Range>::value> VB;

public:
    typedef typename VB::type type;
    static type make (const Range& src) { return VB::make(src); }
};

/** \ingroup BufferRangeTransformationsKthSample */
template <int K, typename Range>
typename kth_sample_range_type<K,Range>::type
kth_sample_range(const Range& src)
{
    return kth_sample_range_type<K,Range>::make(src);
}

}  /* namespace sound */
}  /* namespace psynth */

#endif /* PSYNTH_SOUND_BUFFER_RANGE_FACTORY */
