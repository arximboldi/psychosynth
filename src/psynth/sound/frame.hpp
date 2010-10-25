/**
 *  Time-stamp:  <2010-10-25 19:08:39 raskolnikov>
 *
 *  @file        frame.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 21 13:16:13 2010
 *
 *  Frames, frames are cool.
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

#ifndef PSYNTH_SOUND_FRAME_H_
#define PSYNTH_SOUND_FRAME_H_

#include <functional>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/front.hpp>
#include <boost/type_traits.hpp>

#include <psynth/base/compat.hpp"
#include "channel_base.hpp"
#include "gil_concept.hpp"
#include "sample.hpp"
#include "metafunctions.hpp"
#include "utilities.hpp"
#include "channel_base_algorithm.hpp"

namespace psynth
{
namespace sound
{

/* Forward-declare gray_t */
struct gray_channel_t;
typedef mpl::vector1<gray_channel_t> gray_t;

template <typename FrameBased> struct channel_space_type;
template <typename FrameBased> struct sample_mapping_type;
template <typename FrameBased> struct sample_type;
template <typename FrameBased> struct is_planar;

template <typename FrameBased> struct channel_space_type<const FrameBased> :
	public channel_space_type<FrameBased> {};
template <typename FrameBased>
struct sample_mapping_type<const FrameBased> :
	public sample_mapping_type<FrameBased> {};
template <typename FrameBased> struct sample_type<const FrameBased> :
	public sample_type<FrameBased> {};
template <typename FrameBased> struct is_planar<const FrameBased> :
	public is_planar<FrameBased> {};


template <typename T> struct is_frame : public mpl::false_{};
template <typename T> struct is_frame<const T> : public is_frame<T> {};

/**
   \ingroup FrameBasedAlgorithm
   \brief Returns the number of samples of a frame-based GIL
   construct
*/
template <typename FrameBased>
struct num_samples :
	public mpl::size<typename channel_space_type<FrameBased>::type> {}; 

/**
\addtogroup FrameBasedAlgorithm

Example:
\code
BOOST_STATIC_ASSERT((num_samples<rgb8_view_t>::value==3));
BOOST_STATIC_ASSERT((num_samples<cmyk16_planar_ptr_t>::value==4));

BOOST_STATIC_ASSERT((is_planar<rgb16_planar_image_t>::value));
BOOST_STATIC_ASSERT((is_same<channel_space_type<rgb8_planar_ref_t>::type, rgb_t>::value));
BOOST_STATIC_ASSERT((is_same<sample_mapping_type<cmyk8_frame_t>::type, 
                             sample_mapping_type<rgba8_frame_t>::type>::value));
BOOST_STATIC_ASSERT((is_same<sample_type<bgr8_frame_t>::type, bits8>::value));
\endcode
*/

/**
   \defgroup ChannelBaseModelFrame frame 
   \ingroup ChannelBaseModel
   \brief A homogeneous channel base whose element is a sample
   value. Models HomogeneousChannelBaseValueConcept

   \defgroup FrameModelFrame frame 
   \ingroup FrameModel
   \brief A homogeneous frame value. Models HomogeneousFrameValueConcept

   \ingroup FrameModelFrame ChannelBaseModelFrame FrameBasedModel
   \brief Represents a frame value (a container of samples). Models:
   HomogeneousChannelBaseValueConcept, FrameValueConcept,
   HomogeneousFrameBasedConcept
 
   A frame is a set of samples defining the channel at a given point in
   an image. Conceptually, a frame is little more than a channel base
   whose elements model \p SampleConcept. The class \p frame defines
   a simple, homogeneous frame value. It is used to store the value of
   a channel. The built-in C++ references to \p frame, \p frame& and \p
   const \p frame& are used to represent a reference to a frame inside
   an interleaved image view (a view in which all samples are
   together in memory). Similarly, built-in pointer types \p frame*
   and \p const \p frame* are used as the standard iterator over a row
   of interleaved homogeneous frames.

   Since \p frame inherits the properties of channel base, assigning,
   equality comparison and copy-construcion are allowed between
   compatible frames.  This means that an 8-bit RGB frame may be
   assigned to an 8-bit BGR frame, or to an 8-bit planar
   reference. The samples are properly paired semantically.

   The single-sample (grayscale) instantiation of the class frame,
   (i.e. \p frame<T,gray_layout_t>) is also convertible to/from a
   sample value.  This allows grayscale frames to be used in simpler
   expressions like *gray_pix1 = *gray_pix2 instead of more
   complicated at_c<0>(gray_pix1) = at_c<0>(gray_pix2) or
   get_channel<gray_channel_t>(gray_pix1) =
   get_channel<gray_channel_t>(gray_pix2)
*/
template <typename SampleValue, typename Layout>
// = mpl::range_c<int, 0, ChannelSpace::size> >
struct frame : public detail::homogeneous_channel_base<
    SampleValue, Layout, mpl::size<typename Layout::channel_space_t>::value>
{
private:
    typedef SampleValue sample_t;
    typedef detail::homogeneous_channel_base<
	SampleValue, Layout, mpl::size<typename Layout::channel_space_t>::value>
    parent_t;

public:
    typedef frame                value_type;
    typedef value_type&          reference;
    typedef const value_type&    const_reference;
    BOOST_STATIC_CONSTANT(
	bool, is_mutable = sample_traits<sample_t>::is_mutable);

    frame () {}

    explicit frame (sample_t v)
	: parent_t (v)
    {}  // sets all samples to v

    frame (sample_t v0, sample_t v1)
	: parent_t(v0,v1) {}
    frame(sample_t v0, sample_t v1, sample_t v2)
	: parent_t(v0,v1,v2) {}
    frame(sample_t v0, sample_t v1, sample_t v2, sample_t v3)
	: parent_t(v0,v1,v2,v3) {}
    frame(sample_t v0, sample_t v1, sample_t v2, sample_t v3, sample_t v4)
	: parent_t(v0,v1,v2,v3,v4) {}
    frame(sample_t v0, sample_t v1, sample_t v2, sample_t v3, sample_t v4,
	  sample_t v5)
	: parent_t(v0,v1,v2,v3,v4,v5) {}

    frame (const frame& p)
	: parent_t (p) {}

    frame& operator= (const frame& p)
    {
	static_copy(p,*this);
	return *this;
    }

    /* Construct from another compatible frame type */
    template <typename Frame>
    frame (const Frame& p,
	   typename enable_if_c<is_frame<Frame>::value>::type* dummy = 0)
	: parent_t (p)
    { 
        check_compatible<Frame>();
    }

    template <typename P>
    frame& operator= (const P& p)
    {
	assign (p, mpl::bool_<is_frame<P>::value>());
	return *this;
    }
    
    template <typename P>
    bool operator == (const P& p) const
    {
	return equal (p, mpl::bool_<is_frame<P>::value>());
    } 

    template <typename P>
    bool operator != (const P& p) const
    {
	return !(*this == p);
    }

    /* homogeneous frames have operator[] */
    typename sample_traits<sample_t>::reference
    operator[] (std::size_t i)
    {
	return dynamic_at_c (*this, i);
    }
    
    typename sample_traits<sample_t>::const_reference
    operator[](std::size_t i) const
    {
	return dynamic_at_c (*this, i);
    }

    /* Grayscale support */
    frame&  operator= (sample_t chan)
    {
	check_gray();
	sound::at_c<0>(*this) = chan;
	return *this;
    }
    
    bool operator== (sample_t chan) const
    {
	check_gray ();
	return sound::at_c<0>(*this) == chan;
    }
    
private:
    template <typename Frame>
    void assign (const Frame& p, mpl::true_)
    {
	check_compatible<Frame>();
	static_copy (p, *this);
    }
    
    template <typename Frame>
    bool equal (const Frame& p, mpl::true_) const
    {
	check_compatible<Frame>();
	return static_equal(*this, p);
    } 

    template <typename Frame>
    void check_compatible() const
    {
	gil_function_requires<FramesCompatibleConcept<Frame,frame> >();
    }

    /* Support for assignment/equality comparison of a sample with a
     * grayscale frame */

    static void check_gray()
    {
	BOOST_STATIC_ASSERT(
	    (is_same<typename Layout::channel_space_t, gray_t>::value));
    }
    
    template <typename Sample>
    void assign (const Sample& chan, mpl::false_)
    {
	check_gray ();
	gil::at_c<0> (*this) = chan;
    }
    
    template <typename Sample>
    bool equal (const Sample& chan, mpl::false_) const
    {
	check_gray();
	return gil::at_c<0>(*this)==chan;
    }
};

/*
 *
 *   ChannelBasedConcept
 *
 */

template <typename SampleValue, typename Layout, int K>  
struct kth_element_type<frame<SampleValue,Layout>, K>
{
    typedef SampleValue type;
};

template <typename SampleValue, typename Layout, int K>  
struct kth_element_reference_type<frame<SampleValue,Layout>, K>
{
    typedef typename sample_traits<SampleValue>::reference type;
};

template <typename SampleValue, typename Layout, int K>  
struct kth_element_reference_type<const frame<SampleValue,Layout>, K>
{
    typedef typename sample_traits<SampleValue>::const_reference type;
};

template <typename SampleValue, typename Layout, int K>  
struct kth_element_const_reference_type<frame<SampleValue,Layout>, K>
{
    typedef typename sample_traits<SampleValue>::const_reference type;
};

/*
 *
 *   FrameConcept
 *
 */

template <typename SampleValue, typename Layout> 
struct is_frame<frame<SampleValue,Layout> > : public mpl::true_{};

/*
 *
 *   HomogeneousFrameBasedConcept
 *
 */

template <typename SampleValue, typename Layout>
struct channel_space_type<frame<SampleValue,Layout> >
{
    typedef typename Layout::channel_space_t type;
}; 

template <typename SampleValue, typename Layout>
struct sample_mapping_type<frame<SampleValue,Layout> >
{
    typedef typename Layout::sample_mapping_t type;
}; 

template <typename SampleValue, typename Layout>
struct is_planar<frame<SampleValue,Layout> > :
    public mpl::false_ {};

template <typename SampleValue, typename Layout>
struct sample_type<frame<SampleValue,Layout> >
{
    typedef SampleValue type;
}; 

} /* namespace sound */
} /* namespace psynth */

namespace boost
{
    template <typename SampleValue, typename Layout> 
    struct has_trivial_constructor<psynth::sound::frame<SampleValue,Layout> > :
	public has_trivial_constructor<SampleValue> {};
}

#endif /* PSYNTH_SOUND_FRAME_H_ */
