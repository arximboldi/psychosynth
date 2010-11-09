/**
 *  Time-stamp:  <2010-11-09 15:07:20 raskolnikov>
 *
 *  @file        dynamic_algorithm.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 19:34:10 2010
 *
 *   Some basic STL-style algorithms when applied to runtime type
 *   specified buffer ranges
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

#ifndef PSYNTH_SOUND_DYNAMIC_ALGORITHM_HPP
#define PSYNTH_SOUND_DYNAMIC_ALGORITHM_HPP

#include <psynth/sound/algorithm.hpp>
#include <psynth/sound/dynamic_buffer.hpp>
#include <boost/bind.hpp>

namespace psynth
{
namespace sound
{

/*
 *
 *      equal_frames
 *
 */

namespace detail
{

struct equal_frames_fn : public binary_operation_obj<equal_frames_fn, bool>
{
    template <typename V1, typename V2> PSYNTH_FORCEINLINE
    bool apply_compatible (const V1& v1, const V2& v2) const
    {
	return equal_frames (v1, v2);
    }
};

} /* namespace detail */

/** \ingroup BufferRangeSTLAlgorithmsEqualFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Range2>   // Model MutableBufferRangeConcept
bool equal_frames (const dynamic_buffer_range<Types1>& src, const Range2& dst)
{
    return apply_operation (
	src, boost::bind (detail::equal_frames_fn(), _1, dst));
}

/** \ingroup BufferRangeSTLAlgorithmsEqualFrames */
template <typename Range1,   // Model BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
bool equal_frames (const Range1& src, const dynamic_buffer_range<Types2>& dst)
{
    return apply_operation (
	dst, boost::bind (detail::equal_frames_fn(), src, _1));
}

/** \ingroup BufferRangeSTLAlgorithmsEqualFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
bool equal_frames(const dynamic_buffer_range<Types1>& src,
		  const dynamic_buffer_range<Types2>& dst)
{
    return apply_operation (src, dst, detail::equal_frames_fn());
}


/*
 *
 *      copy_frames
 *
 */

namespace detail
{

struct copy_frames_fn : public binary_operation_obj<copy_frames_fn>
{
    template <typename Range1, typename Range2> PSYNTH_FORCEINLINE
    void apply_compatible (const Range1& src, const Range2& dst) const
    {
	copy_frames(src,dst);
    }
};

} /* namespace detail */

/** \ingroup BufferRangeSTLAlgorithmsCopyFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Range2>   // Model MutableBufferRangeConcept
void copy_frames (const dynamic_buffer_range<Types1>& src, const Range2& dst)
{
    apply_operation (src, boost::bind (detail::copy_frames_fn(), _1, dst));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyFrames */
template <typename Range1,   // Model BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
void copy_frames (const Range1& src, const dynamic_buffer_range<Types2>& dst)
{
    apply_operation (dst, boost::bind (detail::copy_frames_fn(), src, _1));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
void copy_frames (const dynamic_buffer_range<Types1>& src,
		  const dynamic_buffer_range<Types2>& dst)
{
    apply_operation (src, dst, detail::copy_frames_fn());
}


/*
 *
 *      copy_and_convert_frames
 *
 */

// forward declaration for default_channel_converter (see full
// definition in channel_convert.hpp)
struct default_channel_converter;

/** \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Range2,   // Model MutableBufferRangeConcept
          typename CC>      // Model ColorConverterConcept
void copy_and_convert_frames (const dynamic_buffer_range<Types1>& src,
			      const Range2& dst, CC cc)
{
    apply_operation (src, boost::bind (
			 detail::copy_and_convert_frames_fn<CC>(cc), _1, dst));
}

/**  \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Range2>   // Model MutableBufferRangeConcept
void copy_and_convert_frames (const dynamic_buffer_range<Types1>& src,
			      const Range2& dst)
{
    apply_operation (
	src, boost::bind (
	    detail::copy_and_convert_frames_fn<default_channel_converter>(),
	    _1, dst));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Range1,   // Model BufferRangeConcept
          typename Types2,  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
          typename CC>      // Model ColorConverterConcept
void copy_and_convert_frames (const Range1& src,
			      const dynamic_buffer_range<Types2>& dst,
			      CC cc)
{
    apply_operation (
	dst, boost::bind (detail::copy_and_convert_frames_fn<CC> (cc), src, _1));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Range1,   // Model BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
void copy_and_convert_frames (const Range1& src,
			      const dynamic_buffer_range<Types2>& dst)
{
    apply_operation (
	dst, boost::bind (
	    detail::copy_and_convert_frames_fn<default_channel_converter>(),
	    src, _1));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Types2,  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
          typename CC>      // Model ColorConverterConcept
void copy_and_convert_frames (const dynamic_buffer_range<Types1>& src,
			      const dynamic_buffer_range<Types2>& dst,
			      CC cc)
{
    apply_operation (src, dst, detail::copy_and_convert_frames_fn<CC>(cc));
}

/** \ingroup BufferRangeSTLAlgorithmsCopyAndConvertFrames */
template <typename Types1,  // Model MPL Random Access Container of
			    // models of BufferRangeConcept
          typename Types2>  // Model MPL Random Access Container of
			    // models of MutableBufferRangeConcept
void copy_and_convert_frames (const dynamic_buffer_range<Types1>& src,
			      const dynamic_buffer_range<Types2>& dst)
{
    apply_operation (
	src, dst,
	detail::copy_and_convert_frames_fn<default_channel_converter>());
}


/*
 *
 *      fill_frames
 *
 */

namespace detail
{

template <bool Compatible> struct fill_frames_fn1
{
    template <typename V, typename Value>
    static void apply (const V& src, const Value& val)
    {
	fill_frames (src, val);
    }
};

// copy_frames invoked on incompatible buffers
template <>
struct fill_frames_fn1<false>
{
    template <typename V, typename Value>
    static void apply (const V& src, const Value& val)
    {
	throw std::bad_cast();
    }
};

template <typename Value>
struct fill_frames_fn
{
    fill_frames_fn (const Value& val)
	: _val (val) {}

    typedef void result_type;
    template <typename V>
    result_type operator ()(const V& img_range) const
    {
        fill_frames_fn1 <
	    frames_are_compatible<
		typename V::value_type, Value>::value>::apply (img_range,_val);
    }
    Value _val;
};

} /* namespace detail */

/**
 * \ingroup BufferRangeSTLAlgorithmsFillFrames
 * \brief fill_frames for any buffer range. The frame to fill with must
 * be compatible with the current range
 */
template <typename Types, // Model MPL Random Access Container of
			  // models of MutableBufferRangeConcept
          typename Value>
void fill_frames (const dynamic_buffer_range<Types>& range, const Value& val)
{
    apply_operation (range, detail::fill_frames_fn<Value>(val));
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_ALGORITHM */
