/**
 *  Time-stamp:  <2010-11-05 12:01:02 raskolnikov>
 *
 *  @file        algorithm.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Oct 19 13:09:08 2010
 *
 *  Basic STL-like algorithms applied to sound buffers.
 *
 *  @todo With range STL algorithms of C++0x the _frames versions
 *  become trivial. Range based algorithms are not implemented in GCC
 *  yet though (as of GCC 4.4.X).
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

#ifndef PSYNTH_SOUND_ALGORITHM_HPP
#define PSYNTH_SOUND_ALGORITHM_HPP

#include <cassert>
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <memory>
#include <typeinfo>

#include <psynth/base/compat.hpp>
#include <psynth/base/concept.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/channel_base_algorithm.hpp>
#include <psynth/sound/buffer_view.hpp>
#include <psynth/sound/buffer_view_factory.hpp>
#include <psynth/sound/bit_aligned_frame_iterator.hpp>

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4244)
// conversion from 'psynth::image<V,Alloc>::coord_t' to 'int', possible
// loss of data (visual studio compiler doesn't realize that the two
// types are the same)
//#endif

namespace psynth
{
namespace sound
{

template <typename SamplePtr, typename ChannelSpace>
struct planar_frame_iterator;
template <typename Iterator>
class memory_based_step_iterator;

/** A tag denoting incompatible arguments. */
struct error_type {};

/**
   \defgroup ImageViewSTLAlgorithms STL-like Algorithms
   \ingroup ImageViewAlgorithm
   \brief Image view-equivalents of STL algorithms
   
   Image views provide 1D iteration of their frames via \p begin() and
   \p end() methods, which makes it possible to use STL algorithms
   with them. However, using nested loops over X and Y is in many
   cases more efficient. The algorithms in this section resemble STL
   algorithms, but they abstract away the nested loops and take views
   (as opposed to ranges) as input.

   Most algorithms check whether the image views are 1D-traversable. A
   1D-traversable image view has no gaps at the end of the rows. In
   other words, if an x_iterator of that view is advanced past the
   last frame in a row it will move to the first frame of the next
   row. When image views are 1D-traversable, the algorithms use a
   single loop and run more efficiently. If one or more of the input
   views are not 1D-traversable, the algorithms fall-back to an X-loop
   nested inside a Y-loop.

   The algorithms typically delegate the work to their corresponding STL
   algorithms. For example, \p copy_frames calls \p std::copy either for
   each row, or, when the images are 1D-traversable, once for all
   frames.

   In addition, overloads are sometimes provided for the STL
   algorithms. For example, std::copy for planar iterators is overloaded
   to perform \p std::copy for each of the planes. \p std::copy over
   bitwise-copiable frames results in std::copy over unsigned char,
   which STL typically implements via \p memmove.

   As a result \p copy_frames may result in a single call to \p memmove
   for interleaved 1D-traversable views, or one per each plane of planar
   1D-traversable views, or one per each row of interleaved
   non-1D-traversable images, etc.  */

/**
   \defgroup STLOptimizations  Performance overloads of STL algorithms
   \ingroup ImageViewAlgorithm

   \brief overloads of STL algorithms allowing more efficient
   implementation when used with PSYNTH constructs
*/

/**
   \brief A generic binary operation on views
   \ingroup ImageViewSTLAlgorithms

   Use this class as a convenience superclass when defining an
   operation for any image views.  Many operations have different
   behavior when the two views are compatible. This class checks for
   compatibility and invokes apply_compatible(V1,V2) or
   apply_incompatible(V1,V2) of the subclass.  You must provide
   apply_compatible(V1,V2) method in your subclass, but
   apply_incompatible(V1,V2) is not required and the default throws
   std::bad_cast.
*/

template <typename Derived, typename Result = void>
struct binary_operation_obj
{
    typedef Result result_type;

    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    result_type operator () (const std::pair<const V1*, const V2*>& p) const
    {
        return apply (*p.first, *p.second,
		      typename views_are_compatible<V1,V2>::type());
    }

    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    result_type operator () (const V1& v1, const V2& v2) const
    {
        return apply (v1, v2, typename views_are_compatible<V1,V2>::type ());
    }

    result_type operator () (const error_type&) const
    {
	throw std::bad_cast();
    }
    
private:

    // dispatch from apply overload to a function with distinct name
    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    result_type apply(const V1& v1, const V2& v2, boost::mpl::false_) const
    {
        return ((const Derived*) this)->apply_incompatible (v1,v2);
    }

    // dispatch from apply overload to a function with distinct name
    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    result_type apply(const V1& v1, const V2& v2, boost::mpl::true_) const
    {
        return ((const Derived*)this)->apply_compatible(v1,v2);
    }

    // function with distinct name - it can be overloaded by subclasses
    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    result_type apply_incompatible (const V1& v1, const V2& v2) const
    {
        throw std::bad_cast();
    }
};

} /* namespace sound */
} /* namespace psynth */


/**
   \defgroup ImageViewSTLAlgorithmsCopyFrames copy_frames
   \ingroup ImageViewSTLAlgorithms
   \brief std::copy for image views
*/

namespace std
{

/**
   \ingroup STLOptimizations
   \brief Copy when both src and dst are interleaved and of the same
   type can be just memmove
*/
template<typename T, typename Cs> 
PSYNTH_FORCEINLINE psynth::sound::frame<T,Cs>* 
copy (psynth::sound::frame<T,Cs>* first,
      psynth::sound::frame<T,Cs>* last, 
      psynth::sound::frame<T,Cs>* dst)
{ 
    return (psynth::sound::frame<T,Cs>*) std::copy (
	(unsigned char*) first,
	(unsigned char*) last,
	(unsigned char*) dst);
}

/**
   \ingroup STLOptimizations
   \brief Copy when both src and dst are interleaved and of the same
   type can be just memmove
*/
template<typename T, typename Cs> 
PSYNTH_FORCEINLINE psynth::sound::frame<T,Cs>* 
copy (const psynth::sound::frame<T,Cs>* first,
      const psynth::sound::frame<T,Cs>* last, 
      psynth::sound::frame<T,Cs>* dst)
{ 
    return (psynth::sound::frame<T,Cs>*) std::copy(
	(unsigned char*) first,
	(unsigned char*) last,
	(unsigned char*) dst);
}

} /* namespace std */

namespace psynth
{
namespace sound
{
namespace detail
{

template <typename I, typename O>
struct copy_fn
{ 
    PSYNTH_FORCEINLINE I operator() (I first, I last, O dst) const
    {
	return std::copy (first, last, dst);
    } 
};

} /* namespace detail */
} /* namespace sound */
} /* namespace psynth */

namespace std
{

/**
   \ingroup STLOptimizations
   \brief Copy when both src and dst are planar pointers is copy for
   each sample.
*/
template<typename Cs, typename IC1, typename IC2> PSYNTH_FORCEINLINE
psynth::sound::planar_frame_iterator<IC2,Cs>
copy (psynth::sound::planar_frame_iterator<IC1,Cs> first,
      psynth::sound::planar_frame_iterator<IC1,Cs> last,
      psynth::sound::planar_frame_iterator<IC2,Cs> dst)
{ 
    psynth::base::psynth_function_requires <
	psynth::sound::SamplesCompatibleConcept<
	    typename std::iterator_traits<IC1>::value_type,
	    typename std::iterator_traits<IC2>::value_type> >();
    static_for_each (first, last, dst, psynth::sound::detail::copy_fn<IC1,IC2>());
    return dst + (last - first);
}

} // namespace std


namespace psynth
{
namespace sound
{

/**
   \ingroup ImageViewSTLAlgorithmsCopyFrames
   \brief std::copy for image views
*/
template <typename View1, typename View2> PSYNTH_FORCEINLINE
void copy_frames (const View1& src, const View2& dst)
{ 
    assert (src.size () == dst.size ());
    std::copy (src.begin(), src.end(), dst.begin());
}

/**
   \defgroup ImageViewSTLAlgorithmsCopyAndConvertFrames
   copy_and_convert_frames

   \ingroup ImageViewSTLAlgorithms

   \brief copies src view into dst view, channel converting if
   necessary.

   Versions taking static and runtime views are provided. Versions
   taking user-defined channel convered are provided.
*/
namespace detail
{

template <typename CC>
class copy_and_convert_frames_fn :
	public binary_operation_obj <copy_and_convert_frames_fn<CC> >
{
private:
    CC _cc;

public:
    typedef typename
    binary_operation_obj<copy_and_convert_frames_fn<CC> >::result_type
    result_type;

    copy_and_convert_frames_fn () {}

    copy_and_convert_frames_fn (CC cc_in)
	: _cc(cc_in)
    {}
    
    // when the two channel spaces are incompatible, a channel
    // conversion is performed
    template <typename V1, typename V2> PSYNTH_FORCEINLINE 
    result_type apply_incompatible (const V1& src, const V2& dst) const
    {
        copy_frames (channel_converted_view<typename V2::value_type>(src, _cc),
		     dst);
    }

    // If the two channel spaces are compatible, copy_and_convert is
    // just copy
    template <typename V1, typename V2> PSYNTH_FORCEINLINE 
    result_type apply_compatible (const V1& src, const V2& dst) const
    {
         copy_frames (src,dst);
    }
};

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsCopyAndConvertFrames
*/
template <typename V1, typename V2,typename CC> 
PSYNTH_FORCEINLINE 
void copy_and_convert_frames (const V1& src, const V2& dst, CC cc)
{
    detail::copy_and_convert_frames_fn<CC> ccp (cc);
    ccp (src, dst);
}

struct default_channel_converter;

/**
   \ingroup ImageViewSTLAlgorithmsCopyAndConvertFrames
*/
template <typename View1, typename View2> 
PSYNTH_FORCEINLINE 
void copy_and_convert_frames (const View1& src, const View2& dst)
{ 
    detail::copy_and_convert_frames_fn<default_channel_converter> ccp;
    ccp (src,dst);
}

}  /* namespace sound */
}  /* namespace psynth */


/**
   \defgroup ImageViewSTLAlgorithmsFillFrames fill_frames
   \ingroup ImageViewSTLAlgorithms
   \brief std::fill for image views

   @note It seems that for buffers std::fill sufices.
   @note Optimize for planar buffers?
*/

namespace psynth
{
namespace sound
{

namespace detail
{

/** struct to do std::fill */
struct std_fill_type
{
    template <typename It, typename P>
    void operator () (It first, It last, const P& p_in)
    {
        std::fill (first, last, p_in);
    }
};

/** std::fill for planar iterators */ 
template <typename It, typename P>
PSYNTH_FORCEINLINE 
void fill_aux (It first, It last, const P& p, boost::mpl::true_)
{
    static_for_each (first, last, p, std_fill_type ());
}

/** std::fill for interleaved iterators */
template <typename It, typename P>
PSYNTH_FORCEINLINE 
void fill_aux(It first, It last, const P& p, boost::mpl::false_)
{
    std::fill (first, last, p);
}

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsFillFrames
   \brief std::fill for image views
*/
template <typename View, typename Value> PSYNTH_FORCEINLINE 
void fill_frames (const View& buf_view, const Value& val)
{
    detail::fill_aux (buf_view.begin (), buf_view.end (), 
		      val, is_planar<View>());
}


namespace detail
{

template <typename It> PSYNTH_FORCEINLINE
void destruct_range_impl (It first, It last, boost::mpl::true_)
{
    typedef typename std::iterator_traits<It>::value_type value_t;
    if (boost::has_trivial_destructor<value_t>::value)
        return;
    while (first != last) {
        first->~value_t ();
        ++first;
    }
}

template <typename It> PSYNTH_FORCEINLINE
void destruct_range_impl (It, It, boost::mpl::false_) {}

template <typename It> PSYNTH_FORCEINLINE
void destruct_range (It first, It last)
{
    destruct_range_impl (first, last, typename boost::is_pointer<It>::type ());
}

struct std_destruct_t
{
    template <typename It> void operator ()(It first, It last) const
    {
	destruct_range (first,last);
    }
};

/** destruct for planar iterators */
template <typename It>
PSYNTH_FORCEINLINE 
void destruct_aux (It first, It last, boost::mpl::true_)
{
    static_for_each (first, last, std_destruct_t ());
}

/** destruct for interleaved iterators */
template <typename It>
PSYNTH_FORCEINLINE 
void destruct_aux (It first, It last, boost::mpl::false_)
{
    destruct_range (first,last);
}

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsDestructFrames
   \brief Invokes the in-place destructor on every frame of the view
*/
template <typename View> PSYNTH_FORCEINLINE 
void destruct_frames (const View& buf_view)
{
    detail::destruct_aux (buf_view.begin (), buf_view.end (), is_planar<View>());
}


/**
   \defgroup ImageViewSTLAlgorithmsUninitializedFillFrames
   uninitialized_fill_frames
   
   \ingroup ImageViewSTLAlgorithms
   \brief std::uninitialized_fill for image views
*/

namespace detail
{

/**
   std::uninitialized_fill for planar iterators
   If an exception is thrown destructs any in-place copy-constructed
   objects
*/
template <typename It, typename P>
PSYNTH_FORCEINLINE 
void uninitialized_fill_aux (It first, It last,
			     const P& p, boost::mpl::true_)
{
    int sample=0;
    try {
        typedef typename std::iterator_traits<It>::value_type frame_t;
        while (sample < num_samples<frame_t>::value) {
            std::uninitialized_fill (dynamic_at_c (first, sample),
				     dynamic_at_c (last,  sample), 
				     dynamic_at_c (p,     sample));
            ++ sample;
        }
    }
    catch (...)
    {
        for (int c = 0; c < sample; ++c)
            destruct_range (dynamic_at_c (first,c), dynamic_at_c (last,c));
        throw;
    }
}

/**
   std::uninitialized_fill for interleaved iterators
   If an exception is thrown destructs any in-place copy-constructed
   objects
*/
template <typename It, typename P>
PSYNTH_FORCEINLINE 
void uninitialized_fill_aux (It first, It last,
			     const P& p, boost::mpl::false_)
{
    std::uninitialized_fill (first, last, p);
}

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsUninitializedFillFrames
   \brief std::uninitialized_fill for image views.
   Does not support planar heterogeneous views.
   If an exception is thrown destructs any in-place copy-constructed
   frames
*/
template <typename View, typename Value> 
void uninitialized_fill_frames (const View& buf_view, const Value& val)
{
    detail::uninitialized_fill_aux (buf_view.begin(),
				    buf_view.end(), 
				    val, is_planar<View>());
}

/**
   \defgroup ImageViewSTLAlgorithmsDefaultConstructFrames
   default_construct_frames
   
   \ingroup ImageViewSTLAlgorithms
   \brief invokes the default constructor on every frame of an image
   view
*/

namespace detail
{

template <typename It> PSYNTH_FORCEINLINE 
void default_construct_range_impl (It first, It last, boost::mpl::true_)
{
    typedef typename std::iterator_traits<It>::value_type value_t;
    It first1=first;
    try {
        while (first!=last) {
            new (first) value_t ();
            ++first;
        }
    } catch (...) {
        destruct_range (first1, first);
        throw;
    }
}

template <typename It> PSYNTH_FORCEINLINE 
void default_construct_range_impl (It, It, boost::mpl::false_) {}

template <typename It> PSYNTH_FORCEINLINE 
void default_construct_range (It first, It last)
{
    default_construct_range_impl (
	first, last, typename boost::is_pointer<It>::type());
}

/** uninitialized_default_construct for planar iterators */
template <typename It>
PSYNTH_FORCEINLINE 
void default_construct_aux (It first, It last, boost::mpl::true_)
{
    int sample=0;
    try {
        typedef typename std::iterator_traits<It>::value_type frame_t;
        while (sample < num_samples<frame_t>::value) {
            default_construct_range (dynamic_at_c (first, sample),
				     dynamic_at_c (last, sample));
            ++sample;
        }
    } catch (...) {
        for (int c=0; c<sample; ++c)
            destruct_range (dynamic_at_c(first,c), dynamic_at_c(last,c));
        throw;
    }
}

/** uninitialized_default_construct for interleaved iterators */
template <typename It>
PSYNTH_FORCEINLINE 
void default_construct_aux (It first, It last, boost::mpl::false_)
{
    default_construct_range (first, last);
}

template <typename View, bool IsPlanar>
struct has_trivial_frame_constructor :
    public boost::has_trivial_constructor <typename View::value_type> {};

template <typename View>
struct has_trivial_frame_constructor<View, true> :
    public boost::has_trivial_constructor <
    typename sample_type<View>::type> {};

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsDefaultConstructFrames
   \brief Invokes the in-place default constructor on every frame of
   the (uninitialized) view.
   
   Does not support planar heterogeneous views.
   If an exception is thrown destructs any in-place
   default-constructed frames
*/
template <typename View> 
void default_construct_frames (const View& buf_view)
{
    if (detail::has_trivial_frame_constructor<
	    View, is_planar<View>::value>::value)
        return;
 
    detail::default_construct_aux (buf_view.begin (),
				   buf_view.end (),
				   is_planar<View>());
}

namespace detail
{

/** std::uninitialized_copy for pairs of planar iterators */
template <typename It1, typename It2>
PSYNTH_FORCEINLINE 
void uninitialized_copy_aux (It1 first1, It1 last1,
			     It2 first2, boost::mpl::true_)
{
    int sample = 0;
    try {
        typedef typename std::iterator_traits<It1>::value_type frame_t;
        while (sample < num_samples<frame_t>::value) {
            std::uninitialized_copy (dynamic_at_c (first1,sample),
				     dynamic_at_c (last1,sample),
				     dynamic_at_c (first2,sample));
            ++ sample;
        }
    } catch (...) {
        It2 last2 = first2;
        std::advance (last2, std::distance (first1, last1));
        for (int c = 0; c < sample; ++c)
            destruct_range (dynamic_at_c (first2, c),
			    dynamic_at_c (last2, c));
        throw;
    }
}

/** std::uninitialized_copy for interleaved or mixed iterators */
template <typename It1, typename It2>
PSYNTH_FORCEINLINE 
void uninitialized_copy_aux (It1 first1, It1 last1,
			     It2 first2, boost::mpl::false_)
{
    std::uninitialized_copy(first1,last1,first2);
}

} /* namespace detail */

/**
   \ingroup ImageViewSTLAlgorithmsUninitializedCopyFrames
   \brief std::uninitialized_copy for image views.
   Does not support planar heterogeneous views.
   If an exception is thrown destructs any in-place copy-constructed
   objects
*/
template <typename View1, typename View2> 
void uninitialized_copy_frames (const View1& view1, const View2& view2)
{
    typedef boost::mpl::bool_<is_planar<View1>::value &&
		       is_planar<View2>::value> is_planar;
    assert (view1.size () == view2.size ());
    
    detail::uninitialized_copy_aux (view1.begin (), view1.end (), 
				    view2.begin (), is_planar ());
}

/**
   \defgroup ImageViewSTLAlgorithmsForEachFrame for_each_frame
   \ingroup ImageViewSTLAlgorithms
   \brief std::for_each for image views
   
   For contiguous images (i.e. images that have no alignment gap at
   the end of each row) it is more efficient to use the underlying
   frame iterator that does not check for the end of rows.  For
   non-contiguous images for_each_frame resolves to for_each of each
   row using the underlying frame iterator, which is still faster

   \ingroup ImageViewSTLAlgorithmsForEachFrame
*/
template <typename V, typename F>
F for_each_frame (const V& buf, F fun)
{
    return std::for_each (buf.begin (), buf.end(), fun);
}

/**
   \defgroup ImageViewSTLAlgorithmsForEachFramePosition
   for_each_frame_position
   
   \ingroup ImageViewSTLAlgorithms
   \brief adobe::for_each_position for image views (passes locators,
   instead of frame references, to the function object)
   
   \ingroup ImageViewSTLAlgorithmsForEachFramePosition
*/

template <typename View, typename F>
F for_each_frame_position (const View& buf, F fun)
{
    for (auto loc = buf.begin (); loc != buf.end (); ++loc)
	fun (loc);
    
    return fun;
}

/**
   \defgroup ImageViewSTLAlgorithmsGenerateFrames generate_frames
   \ingroup ImageViewSTLAlgorithms
   \brief std::generate for image views

   \ingroup ImageViewSTLAlgorithmsGenerateFrames
   \brief std::generate for image views
*/
template <typename View, typename F>
void generate_frames (const View& v, F fun)
{
    std::generate (v.begin (), v.end (), fun);
}

/**
   \defgroup ImageViewSTLAlgorithmsEqualFrames equal_frames
   \ingroup ImageViewSTLAlgorithms
   \brief std::equal for image views
*/
template <typename I1, typename I2> PSYNTH_FORCEINLINE
bool equal_n (I1 i1, std::ptrdiff_t n, I2 i2);

namespace detail
{

template <typename I1, typename I2>
struct equal_n_fn
{
    PSYNTH_FORCEINLINE
    bool operator () (I1 i1, std::ptrdiff_t n, I2 i2) const
    {
	return std::equal (i1, i1+n, i2);
    }
};

/**
   Equal when both ranges are interleaved and of the same type. 
   PSYNTH frames are bitwise comparable, so memcmp is used. User-defined
   frames that are not bitwise comparable need to provide an overload
*/
template<typename T, typename Cs>
struct equal_n_fn<const frame<T,Cs>*, const frame<T,Cs>*>
{
    PSYNTH_FORCEINLINE
    bool operator () (const frame<T,Cs>* i1, std::ptrdiff_t n,
		      const frame<T,Cs>* i2) const
    { 
        return memcmp (i1, i2, n * sizeof (frame<T,Cs>)) == 0;
    }
};

template<typename T, typename Cs>
struct equal_n_fn<frame<T,Cs>*, frame<T,Cs>*> :
	equal_n_fn<const frame<T,Cs>*, const frame<T,Cs>*> {};

/**
   EqualFrames
   Equal when both ranges are planar pointers of the same
   type. memcmp is invoked for each sample plane
   User-defined samples that are not bitwise comparable need to
   provide an overload
*/
template<typename IC, typename Cs>
struct equal_n_fn<planar_frame_iterator<IC,Cs>,
		  planar_frame_iterator<IC,Cs> >
{
    PSYNTH_FORCEINLINE
    bool operator () (const planar_frame_iterator<IC,Cs> i1,
		      std::ptrdiff_t n,
		      const planar_frame_iterator<IC,Cs> i2) const
    { 
        ptrdiff_t num_bytes = n * sizeof (
	    typename std::iterator_traits<IC>::value_type);

        for (std::ptrdiff_t i=0; i<boost::mpl::size<Cs>::value; ++i)
            if (memcmp (dynamic_at_c (i1, i),
			dynamic_at_c (i2, i), num_bytes) != 0)
                return false;
        return true;
    }
};

} /* namespace detail */

template <typename I1, typename I2> PSYNTH_FORCEINLINE
bool equal_n (I1 i1, std::ptrdiff_t n, I2 i2)
{
    return detail::equal_n_fn<I1,I2>() (i1, n, i2);
}

} /* namespace sound */
} /* namespace psynth */

namespace std
{
/**
   \ingroup STLOptimizations
   \brief  std::equal(I1,I1,I2) with I1 and I2 being a iterator_from_2d

   Invoked when one calls std::equal(I1,I1,I2) with I1 and I2 being a
   iterator_from_2d (which is a 1D iterator over the frames in an
   image). Attempts to demote the source and destination iterators to
   simpler/faster types if the corresponding range is contiguous.  For
   contiguous images (i.e. images that have no alignment gap at the
   end of each row) it is more efficient to use the underlying frame
   iterator that does not check for the end of rows. If the underlying
   frame iterator happens to be a fundamental planar/interleaved
   pointer, the call may further resolve to memcmp. Otherwise it
   resolves to copying each row using the underlying frame iterator.

   @todo We removed this overload because it was based on
   iterator_from_2d. Please provide overloads so equal_n_fn is used
   instead of default std::equal
*/

} /* namespace std */

namespace psynth
{
namespace sound
{

/**
   \ingroup ImageViewSTLAlgorithmsEqualFrames
   \brief std::equal for image views
*/
template <typename View1, typename View2> PSYNTH_FORCEINLINE 
bool equal_frames (const View1& v1, const View2& v2)
{
    assert (v1.size() == v2.size());
    return std::equal (v1.begin (), v1.end (), v2.begin ());
    // std::equal has overloads with PSYNTH iterators for optimal
    // performance
    // @todo Not anymore actually. 
}

/**
   \defgroup ImageViewSTLAlgorithmsTransformFrames transform_frames
   \ingroup ImageViewSTLAlgorithms
   \brief std::transform for image views

   \ingroup ImageViewSTLAlgorithmsTransformFrames
   \brief std::transform for image views

   @todo Implement with STL?
*/
template <typename View1, typename View2, typename F> PSYNTH_FORCEINLINE 
F transform_frames (const View1& src, const View2& dst, F fun)
{
    assert (src.size() == dst.size());
    for (std::ptrdiff_t x = 0; x < src.size (); ++x)
	dst [x] = fun (src [x]);
    return fun;
}

/**
   \ingroup ImageViewSTLAlgorithmsTransformFrames
   \brief transform_frames with two sources
*/
template <typename View1, typename View2, typename View3, typename F>
PSYNTH_FORCEINLINE 
F transform_frames (const View1& src1, const View2& src2,
		    const View3& dst, F fun)
{
    assert (src1.size () == dst.size() &&
	    src2.size () == dst.size ());
    for (std::ptrdiff_t x = 0; x < dst.size (); ++x)
            dst [x] = fun (src1 [x], src2 [x]);
    return fun;
}

/**
   \defgroup ImageViewSTLAlgorithmsTransformFramePositions
   transform_frame_positions
   \ingroup ImageViewSTLAlgorithms
   \brief adobe::transform_positions for image views (passes locators,
   instead of frame references, to the function object)
   
   \ingroup ImageViewSTLAlgorithmsTransformFramePositions
   \brief Like transform_frames but passes to the function object
   frame locators instead of frame references
*/ 
template <typename View1, typename View2, typename F> PSYNTH_FORCEINLINE 
F transform_frame_positions (const View1& src,const View2& dst, F fun)
{
    assert (src.size () == dst.size ());
    typename View1::iterator loc = src.begin ();

    for (std::ptrdiff_t x = 0; x < src.size (); ++x, ++loc)
	dst [x] = fun (loc);
        
    return fun;
}

/**
   \ingroup ImageViewSTLAlgorithmsTransformFramePositions
   \brief transform_frame_positions with two sources
*/
template <typename View1, typename View2, typename View3, typename F>
PSYNTH_FORCEINLINE 
F transform_frame_positions (const View1& src1,
			     const View2& src2,
			     const View3& dst, F fun)
{
    assert (src1.size () == dst.size ());
    assert (src2.size () == dst.size ());
    typename View1::iterator loc1 = src1.begin ();
    typename View2::iterator loc2 = src2.begin ();
    for (std::ptrdiff_t x = 0; x < src1.width();
	 ++x, ++loc1, ++loc2)
	dst [x] = fun (loc1, loc2);
        
    return fun;
}

}  /* namespace sound */
}  /* namespace psynth */

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

#endif /* PSYNTH_SOUND_ALGORITHM */
