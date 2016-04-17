/**
 *  Time-stamp:  <2010-11-11 21:25:33 raskolnikov>
 *
 *  @file        device_n.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Oct 29 15:19:40 2010
 *
 *  Support for color space of N channels and variants
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

#ifndef PSYNTH_SOUND_DEVICE_N_H
#define PSYNTH_SOUND_DEVICE_N_H

#include <cstddef>
#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>
#include <psynth/sound/metafunctions.hpp>

namespace psynth
{
namespace sound
{

/**
   \brief unnamed color
   \ingroup ColorNameModel
*/
template <int N>
struct devicen_channel {};

template <int N>
struct devicen_space;

/**
   \brief unnamed color space of one channel
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<1> : public boost::mpl::vector1<devicen_channel<0> > {};

/**
   \brief unnamed color space of two channels
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<2> : public boost::mpl::vector2<
    devicen_channel<0>,
    devicen_channel<1> > {};

/**
   \brief unnamed color space of three channels
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<3> : public boost::mpl::vector3<
    devicen_channel<0>,
    devicen_channel<1>,
    devicen_channel<2> > {};

/**
   \brief unnamed color space of four channels
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<4> : public boost::mpl::vector4<
    devicen_channel<0>,
    devicen_channel<1>,
    devicen_channel<2>,
    devicen_channel<3> > {};

/**
   \brief unnamed color space of five channels
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<5> : public boost::mpl::vector5<
    devicen_channel<0>,
    devicen_channel<1>,
    devicen_channel<2>,
    devicen_channel<3>,
    devicen_channel<4> > {};

/**
   \brief unnamed color layout of up to five channels
   \ingroup LayoutModel
*/
template <int N> struct devicen_layout : public layout<devicen_space<N> > {};

/**
   \ingroup ImageViewConstructors
   \brief from 2-channel planar data
*/
template <typename IC>
inline typename type_from_iterator<
    planar_frame_iterator<IC,devicen_space<2> > >::range
planar_devicen_range (std::size_t size, IC c0, IC c1)
{
    typedef typename type_from_iterator<planar_frame_iterator<
	IC, devicen_space<2> > >::range range;
    return range (size, typename range::iterator (c0, c1));
}

/**
   @todo Implement range factories for more dimensions.
 */

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DEVICEN */
