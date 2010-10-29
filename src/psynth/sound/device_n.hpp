/**
 *  Time-stamp:  <2010-10-29 15:23:31 raskolnikov>
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
struct devicen_space<1> : public mpl::vector1<devicen_channel<0> > {};

/**
   \brief unnamed color space of two channels
   \ingroup ColorSpaceModel
*/
template <>
struct devicen_space<2> : public mpl::vector2<devicen_channel<0>,
					  devicen_channel<1> > {};

/// \brief unnamed color space of three channels
/// \ingroup ColorSpaceModel
template <> struct devicen_space<3> : public mpl::vector3<devicen_channel<0>, devicen_channel<1>, devicen_channel<2> > {};

/// \brief unnamed color space of four channels
/// \ingroup ColorSpaceModel
template <> struct devicen_space<4> : public mpl::vector4<devicen_channel<0>, devicen_channel<1>, devicen_channel<2>, devicen_channel<3> > {};

/// \brief unnamed color space of five channels
/// \ingroup ColorSpaceModel
template <> struct devicen_space<5> : public mpl::vector5<devicen_channel<0>, devicen_channel<1>, devicen_channel<2>, devicen_channel<3>, devicen_channel<4> > {};

/// \brief unnamed color layout of up to five channels
/// \ingroup LayoutModel
template <int N> struct devicen_layout : public layout<devicen_space<N> > {};

/// \ingroup ImageViewConstructors
/// \brief from 2-channel planar data
template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<2> > >::view_t
planar_devicen_view(std::size_t width, std::size_t height, IC c0, IC c1, std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<2> > >::view_t view_t;
    return view_t(width, height, typename view_t::locator(typename view_t::x_iterator(c0,c1), rowsize_in_bytes));
}

/// \ingroup ImageViewConstructors
/// \brief from 3-channel planar data
template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<3> > >::view_t
planar_devicen_view(std::size_t width, std::size_t height, IC c0, IC c1, IC c2, std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<3> > >::view_t view_t;
    return view_t(width, height, typename view_t::locator(typename view_t::x_iterator(c0,c1,c2), rowsize_in_bytes));
}

/// \ingroup ImageViewConstructors
/// \brief from 4-channel planar data
template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<4> > >::view_t
planar_devicen_view(std::size_t width, std::size_t height, IC c0, IC c1, IC c2, IC c3, std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<4> > >::view_t view_t;
    return view_t(width, height, typename view_t::locator(typename view_t::x_iterator(c0,c1,c2,c3), rowsize_in_bytes));
}

/// \ingroup ImageViewConstructors
/// \brief from 5-channel planar data
template <typename IC>
inline typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<5> > >::view_t
planar_devicen_view(std::size_t width, std::size_t height, IC c0, IC c1, IC c2, IC c3, IC c4, std::ptrdiff_t rowsize_in_bytes) {
    typedef typename type_from_x_iterator<planar_pixel_iterator<IC,devicen_space<5> > >::view_t view_t;
    return view_t(width, height, typename view_t::locator(typename view_t::x_iterator(c0,c1,c2,c3,c4), rowsize_in_bytes));
}

} }  // namespace boost::gil

#endif
