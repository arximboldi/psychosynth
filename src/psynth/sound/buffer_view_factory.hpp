/**
 *  Time-stamp:  <2010-11-05 12:08:26 raskolnikov>
 *
 *  @file        buffer_view_factory.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Oct 29 11:55:36 2010
 *
 *  View factories.  Methods for creating shallow image views from raw
 *  pixel data or from other image views - flipping horizontally or
 *  vertically, axis-aligned rotation, a subimage, sub_sampled or n-th
 *  channel image view. Derived image views are shallow copies and are
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

#ifndef PSYNTH_SOUND_BUFFER_VIEW_FACTORY_HPP
#define PSYNTH_SOUND_BUFFER_VIEW_FACTORY_HPP

#include <cassert>
#include <cstddef>

#include <psynth/base/compat.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/mono.hpp>
#include <psynth/sound/channel_convert.hpp>

/**
   \defgroup BufferViewConstructors Buffer View From Raw Data
   \ingroup BufferViewAlgorithm
   \brief Methods for constructing buffer views from raw data and for
   getting raw data from views
*/

/**
   \defgroup BufferViewTransformations Buffer View Transformations
   \ingroup BufferViewAlgorithm
   \brief Methods for constructing one buffer view from another
*/

namespace psynth
{
namespace sound
{
struct default_channel_converter;

template <typename T> struct dynamic_step_type;
template <typename T> struct transposed_type;

/**
   \ingroup BufferViewConstructors
   \brief Constructing buffer views from raw interleaved frame data
*/
template <typename Iterator>
typename type_from_iterator<Iterator>::view
interleaved_view (std::size_t size, Iterator frames)
{
    typedef typename type_from_iterator<Iterator>::view RView;
    return RView (size, typename RView::iterator (frames));
}

/**
   interleaved_view_get_raw_data, planar_view_get_raw_data - return
   pointers to the raw data (the samples) of a basic homogeneous view.
*/

namespace detail
{

template <typename View, bool IsMutable>
struct sample_pointer_type_impl;

template <typename View> struct sample_pointer_type_impl<View, true>
{
    typedef typename sample_type<View>::type* type;
};

template <typename View> struct sample_pointer_type_impl<View, false>
{
    typedef const typename sample_type<View>::type* type;
};

template <typename View> struct sample_pointer_type
    : public sample_pointer_type_impl<View, view_is_mutable<View>::value> {};

} /* namespace detail */


/**
   \ingroup BufferViewConstructors
   \brief Returns C pointer to the the samples of an interleaved
   homogeneous view.
*/
template <typename HomogeneousView>
typename detail::sample_pointer_type<HomogeneousView>::type
interleaved_view_get_raw_data(const HomogeneousView& view)
{
    BOOST_STATIC_ASSERT((!is_planar<HomogeneousView>::value &&
			 view_is_basic<HomogeneousView>::value));
    BOOST_STATIC_ASSERT((boost::is_pointer<
			     typename HomogeneousView::iterator>::value));

    return &psynth::sound::at_c<0>(view [0]);
}

/**
   \ingroup BufferViewConstructors
   \brief Returns C pointer to the the samples of a given channel
   plane of a planar homogeneous view.
*/
template <typename HomogeneousView>
typename detail::sample_pointer_type<HomogeneousView>::type
planar_view_get_raw_data(const HomogeneousView& view, int plane_index)
{
    BOOST_STATIC_ASSERT((is_planar<HomogeneousView>::value &&
			 view_is_basic<HomogeneousView>::value));
    return dynamic_at_c (view.begin (), plane_index);
}


/**
   \defgroup BufferViewTransformationsChannelConvert channel_converted_view
   \ingroup BufferViewTransformations
   \brief Channel converted view of another view

   \ingroup BufferViewTransformationsChannelConvert FrameDereferenceAdaptorModel
   \brief Function object that given a source frame, returns it converted to a given channel space and sample depth. Models: FrameDereferenceAdaptorConcept
   
   Useful in constructing a channel converted view over a given buffer view
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
template <typename SrcView, typename CC, typename DstP, typename SrcP>
struct channel_converted_view_type_impl
{
private:
    typedef channel_convert_deref_fn<
    typename SrcView::const_type::reference,DstP,CC> deref_t;
    typedef typename SrcView::template add_deref<deref_t> add_ref_t;
    
public:
    typedef typename add_ref_t::type type;
        static type make (const SrcView& sv,CC cc)
    {
	return add_ref_t::make (sv, deref_t (cc));
    }
};

/* If the Src view has the same frame type as the target, there is
   no need for channel conversion */
template <typename SrcView, typename CC, typename DstP>
struct channel_converted_view_type_impl<SrcView,CC,DstP,DstP>
{
    typedef SrcView type;
    static type make (const SrcView& sv, CC)
    { return sv; }
};

} /* namespace detail */


/**
   \brief Returns the type of a view that does channel conversion upon
   dereferencing its frames
   
   \ingroup BufferViewTransformationsChannelConvert
*/
template <typename SrcView, typename DstP, typename CC=default_channel_converter>
struct channel_converted_view_type :
    public detail::channel_converted_view_type_impl<SrcView,
						    CC,
						    DstP,
						    typename SrcView::value_type>
{
    PSYNTH_CLASS_REQUIRE(DstP, psynth::sound, MutableFrameConcept)
    // Why does it have to be mutable???
};


/**
   \ingroup BufferViewTransformationsChannelConvert
   \brief view of a different channel space with a user defined
   channel-converter
*/
template <typename DstP, typename View, typename CC>
inline typename channel_converted_view_type<View,DstP,CC>::type
channel_converted_view (const View& src, CC cc)
{
    return channel_converted_view_type<View,DstP,CC>::make(src,cc);
}

/**
   \ingroup BufferViewTransformationsChannelConvert
   \brief overload of generic channel_converted_view with the default
   channel-converter
*/
template <typename DstP, typename View>
inline typename channel_converted_view_type<View,DstP>::type
channel_converted_view(const View& src)
{
    return channel_converted_view<DstP>(src, default_channel_converter());
}


/**
   \defgroup BufferViewTransformationsFlipLR flipped_left_right_view
   \ingroup BufferViewTransformations
   \brief view of a view flipped left-to-right

   \ingroup BufferViewTransformationsFlipLR
*/
template <typename View> 
inline typename dynamic_step_type<View>::type flipped_view (const View& src)
{
    typedef typename dynamic_step_type<View>::type RView;
    return RView (src.size (), make_step_iterator (src.end () - 1, -1));
}

/**
   \defgroup BufferViewTransformationsSubbuffer subbuffer_view
   \ingroup BufferViewTransformations
   \brief view of an axis-aligned rectangular area within an buffer_view

   \ingroup BufferViewTransformationsSubbuffer
*/
template <typename View> 
inline View sub_buffer_view (const View& src,
			     const typename View::size_type start,
			     const typename View::size_type size)
{
    return View (size, src.at (start));
}

/**
   \defgroup BufferViewTransformationsSub_Sampled sub_sampled_view
   \ingroup BufferViewTransformations
   \brief view of a sub_sampled version of an buffer_view, stepping
   over a number of samples in X and number of rows in Y

   \ingroup BufferViewTransformationsSub_Sampled
*/
template <typename View> 
inline typename dynamic_step_type<View>::type
sub_sampled_view (const View& src, typename View::size_type step)
{
    assert (step > 0);
    typedef typename dynamic_step_type<View>::type RView;
    return RView ((src.size () + (step - 1)) / step,
		  make_step_iterator (src.begin (), step));
}


/**
   \defgroup BufferViewTransformationsNthSample nth_sample_view
   \ingroup BufferViewTransformations
   \brief single-sample (grayscale) view of the N-th sample of a given
   buffer_view
*/

namespace detail
{
template <typename View, bool AreSamplesTogether>
struct nth_sample_view_basic_impl;

/**
   nth_sample_view when the samples are not adjacent in memory. This
   can happen for multi-sample interleaved buffers or buffers with a
   step
*/
template <typename View>
struct nth_sample_view_basic_impl<View, false>
{
    typedef typename view_type<typename sample_type<View>::type,
			       mono_layout, false, true,
			       view_is_mutable<View>::value>::type type;

    static type make (const View& src, int n)
    {
	typedef typename type::iterator iterator;
	typedef typename iterator_adaptor_get_base <iterator>::type iterator_base;
	
	return type (src.size (),
		     iterator (iterator_base (& (src [0] [n])),
			       src.frame_size ()));
    }
};

/*
  nth_sample_view when the samples are together in memory (true for
  simple grayscale or planar buffers)
*/
template <typename View>
struct nth_sample_view_basic_impl<View, true>
{
    typedef typename view_type<typename sample_type<View>::type,
			       mono_layout, false, false,
			       view_is_mutable<View>::value>::type type;

    static type make (const View& src, int n)
    {
	typedef typename type::iterator iterator;
	return interleaved_view (src.size (), (iterator) &(src [0][n]));
    }
};

template <typename View, bool IsBasic>
struct nth_sample_view_impl;

// For basic (memory-based) views dispatch to nth_sample_view_basic_impl
template <typename View> struct nth_sample_view_impl<View,true>
{
private:
    typedef typename View::iterator src_iterator;

    // Determines whether the samples of a given frame iterator are
    // adjacent in memory.  Planar and grayscale iterators have
    // samples adjacent in memory, whereas multi-sample interleaved
    // and iterators with non-fundamental step do not.
    BOOST_STATIC_CONSTANT(bool, adjacent=
			  !iterator_is_step<src_iterator>::value &&
			  (is_planar<src_iterator>::value ||
			   num_samples<View>::value==1));
public:
    typedef typename nth_sample_view_basic_impl <View, adjacent>::type type;

    static type make (const View& src, int n)
    {
	return nth_sample_view_basic_impl <View,adjacent>::make (src,n);
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

template <typename View>
struct nth_sample_view_impl<View,false>
{
private:
    typedef nth_sample_deref_fn<typename View::reference> deref_t;
    typedef typename View::template add_deref<deref_t>    AD;
    
public:
    typedef typename AD::type type;
    static type make(const View& src, int n)
    {
	return AD::make(src, deref_t(n));
    }
};

} /* namespace detail */

/**
   \brief Given a source buffer view type View, returns the type of an
   buffer view over a single sample of View
   \ingroup BufferViewTransformationsNthSample
   
   If the samples in the source view are adjacent in memory (such as
   planar non-step view or single-sample view) then the return view is
   a single-sample non-step view.  If the samples are non-adjacent
   (interleaved and/or step view) then the return view is a
   single-sample step view.
*/
template <typename View>
struct nth_sample_view_type
{
private:
    PSYNTH_CLASS_REQUIRE(View, psynth::sound, BufferViewConcept)
    typedef detail::nth_sample_view_impl<View, view_is_basic<View>::value> VB;

public:
    typedef typename VB::type type;
    static type make(const View& src, int n) { return VB::make(src,n); }
};


/**
   \ingroup BufferViewTransformationsNthSample
*/
template <typename View>
typename nth_sample_view_type<View>::type
nth_sample_view (const View& src, int n)
{
    return nth_sample_view_type<View>::make (src,n);
}


/**
   \defgroup BufferViewTransformationsKthSample kth_sample_view
   \ingroup BufferViewTransformations
   \brief single-sample (grayscale) view of the K-th sample of a given
   buffer_view. The sample index is a template parameter
*/

namespace detail
{

template <int K, typename View, bool AreSamplesTogether>
struct kth_sample_view_basic_impl;

/*
  kth_sample_view when the samples are not adjacent in memory. This
  can happen for multi-sample interleaved buffers or buffers with a
  step
*/
template <int K, typename View>
struct kth_sample_view_basic_impl <K, View, false>
{
private:
    typedef typename kth_element_type<typename View::value_type,K>::type sample_t;

public:
    typedef typename view_type<sample_t, mono_layout, false, true,
			       view_is_mutable<View>::value>::type type;

    static type make(const View& src)
    {
	typedef typename type::iterator iterator;
	typedef typename iterator_adaptor_get_base<iterator>::type iterator_base;
	return type(src.size (),
		    iterator (iterator_base (&sound::at_c<K> (src [0])),
			      src.frame_size ()));
    }
};

/*
  kth_sample_view when the samples are together in memory (true for
  simple grayscale or planar buffers)
*/
template <int K, typename View>
struct kth_sample_view_basic_impl <K, View, true>
{
private:
    typedef typename kth_element_type<typename View::value_type, K>::type sample_t;

public:
    typedef typename view_type<
    sample_t, mono_layout, false, false, view_is_mutable<View>::value>::type type;

    static type make (const View& src)
    {
	typedef typename type::iterator iterator;
	return interleaved_view (src.size (), (iterator) &sound::at_c<K> (src[0]));
    }
};

template <int K, typename View, bool IsBasic>
struct kth_sample_view_impl;

/* For basic (memory-based) views dispatch to __kth_sample_view_basic */
template <int K, typename View>
struct kth_sample_view_impl <K, View, true>
{
private:
    typedef typename View::iterator src_iterator;

    // Determines whether the samples of a given frame iterator are
    // adjacent in memory.  Planar and grayscale iterators have
    // samples adjacent in memory, whereas multi-sample interleaved
    // and iterators with non-fundamental step do not.
    BOOST_STATIC_CONSTANT(bool, adjacent=
			  !iterator_is_step<src_iterator>::value &&
			  (is_planar<src_iterator>::value ||
			   num_samples<View>::value==1));

public:
    typedef typename kth_sample_view_basic_impl <K, View, adjacent>::type type;

    static type make (const View& src)
    {
	return kth_sample_view_basic_impl <K, View, adjacent>::make (src);
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

template <int K, typename View>
struct kth_sample_view_impl<K,View,false>
{
private:
    typedef kth_sample_deref_fn<K,typename View::reference> deref_t;
    typedef typename View::template add_deref<deref_t>   AD;

public:
    typedef typename AD::type type;
    static type make (const View& src) { return AD::make (src, deref_t ()); }
};

} /* namespace detail */

/**
   \brief Given a source buffer view type View, returns the type of an
   buffer view over a given sample of View.
   
   \ingroup BufferViewTransformationsKthSample
   
   If the samples in the source view are adjacent in memory (such as
   planar non-step view or single-sample view) then the return view is
   a single-sample non-step view.  If the samples are non-adjacent
   (interleaved and/or step view) then the return view is a
   single-sample step view.
*/
template <int K, typename View>
struct kth_sample_view_type
{
private:
    PSYNTH_CLASS_REQUIRE(View, psynth::sound, BufferViewConcept)
    typedef detail::kth_sample_view_impl<K, View, view_is_basic<View>::value> VB;

public:
    typedef typename VB::type type;
    static type make (const View& src) { return VB::make(src); }
};

/** \ingroup BufferViewTransformationsKthSample */
template <int K, typename View>
typename kth_sample_view_type<K,View>::type
kth_sample_view(const View& src)
{
    return kth_sample_view_type<K,View>::make(src);
}

}  /* namespace sound */
}  /* namespace psynth */

#endif /* PSYNTH_SOUND_BUFFER_VIEW_FACTORY */
