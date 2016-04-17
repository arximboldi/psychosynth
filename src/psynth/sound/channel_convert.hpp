/**
 *  Time-stamp:  <2011-03-21 17:31:00 raskolnikov>
 *
 *  @file        channel_convert.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Nov  2 10:27:57 2010
 *
 *   default channel space conversions
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

#ifndef PSYNTH_SOUND_CHANNEL_CONVERT_HPP
#define PSYNTH_SOUND_CHANNEL_CONVERT_HPP

#include <functional>

#include <psynth/base/compat.hpp>

#include <psynth/sound/sample_algorithm.hpp>
#include <psynth/sound/frame.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/util.hpp>
#include <psynth/sound/channel_base_algorithm.hpp>

#include <psynth/sound/mono.hpp>
#include <psynth/sound/stereo.hpp>
#include <psynth/sound/quad.hpp>
#include <psynth/sound/surround.hpp>

namespace psynth
{
namespace sound
{

// Forward-declare
template <typename P>
struct sample_type;

/*
 *
 *                 CHANNEL SPACE CONVERSION
 *
 *
 */

/**
   \ingroup ChannelConvert
   \brief Channel Convertion function object. To be specialized for
   every src/dst channel space
*/
template <typename C1, typename C2>
struct default_channel_converter_impl {};

/**
   \ingroup ChannelConvert
   \brief When the channel space is the same, channel convertion
   performs sample depth conversion
*/
template <typename C>
struct default_channel_converter_impl<C, C>
{
    template <typename P1, typename P2>
    void operator () (const P1& src, P2& dst) const
    {
        static_for_each (src, dst, default_sample_converter());
    }
};


/**
   \ingroup ChannelConvert
   \brief Mono to Stereo
*/
template <>
struct default_channel_converter_impl<mono_space, stereo_space>
{
    template <typename P1, typename P2>
    void operator () (const P1& src, P2& dst) const
    {
        get_channel (dst, left_channel ())  =
            sample_convert<typename channel_element_type<P2,left_channel>::type>(
		get_channel (src, mono_channel ()));
	get_channel (dst, right_channel ())  =
            sample_convert<typename channel_element_type<P2,right_channel>::type>(
		get_channel (src, mono_channel ()));
    }
};

/**
   \ingroup ChannelConvert
   \brief Stereo to Mono
*/
template <>
struct default_channel_converter_impl<stereo_space, mono_space>
{
    template <typename P1, typename P2>
    void operator() (const P1& src, P2& dst) const
    {
        get_channel (dst, mono_channel ()) =
	    sample_convert<typename channel_element_type<P2, mono_channel>::type>(
                get_channel (src, left_channel ()) / 2 +
		get_channel (src, right_channel ()) / 2
            );
    }
};

/**
   @defgroup ChannelConvert Color Space Converion
   \ingroup ChannelSpaces
   \brief Support for conversion between frames of different channel
   spaces and sample depths

   \ingroup FrameAlgorithm ChannelConvert
   \brief class for channel-converting one frame to another
*/
struct default_channel_converter
{
    template <typename SrcP, typename DstP>
    void operator () (const SrcP& src,DstP& dst) const
    {
        typedef typename channel_space_type<SrcP>::type SrcChannelSpace;
        typedef typename channel_space_type<DstP>::type DstChannelSpace;
        default_channel_converter_impl<SrcChannelSpace, DstChannelSpace>()(
	    src, dst);
    }
};

/**
   \ingroup FrameAlgorithm
   \brief helper function for converting one frame to another using
   GIL default channel-converters

     where ScrP models HomogeneousFrameConcept
           DstP models HomogeneousFrameValueConcept
*/
template <typename SrcP, typename DstP>
inline void channel_convert (const SrcP& src, DstP& dst)
{
    default_channel_converter()(src,dst);
}

template <typename SrcP, typename DstP>
inline DstP channel_convert (const SrcP& src)
{
    DstP res;
    default_channel_converter()(src,res);
    return res;
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_CHANNEL_CONVERT */
