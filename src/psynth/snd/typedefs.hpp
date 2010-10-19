/**
 *  Time-stamp:  <2010-10-19 13:08:57 raskolnikov>
 *
 *  @file        typedefs.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Oct 19 13:04:44 2010
 *
 *  Useful typedefs.
 *
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
 *
 *  This file is part of Psychosynh.
 *   
 *  Psychosynh is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynh is distributed in the hope that it will be useful,
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

#ifndef PSYNTH_TYPEDEFS_H
#define PSYNTH_TYPEDEFS_H

#include "gil_config.hpp"
#include <boost/cstdint.hpp>
#include "gray.hpp"
#include "rgb.hpp"
#include "rgba.hpp"
#include "cmyk.hpp"
#include "device_n.hpp"
#include <memory>

// CS = 'bgr' LAYOUT='bgr_layout_t'
#define PSYNTH_SND_DEFINE_BASE_TYPEDEFS_INTERNAL(T,CS,LAYOUT)			\
    template <typename, typename>    struct pixel;			\
    template <typename, typename>    struct planar_pixel_reference;	\
    template <typename, typename>    struct planar_pixel_iterator;	\
    template <typename>              class memory_based_step_iterator;	\
    template <typename>              class point2;			\
    template <typename>              class memory_based_2d_locator;	\
    template <typename>              class image_view;			\
    template <typename, bool, typename>     class image;		\
    typedef pixel<bits##T, LAYOUT >         CS##T##_pixel_t;		\
    typedef const pixel<bits##T, LAYOUT >   CS##T##c_pixel_t;		\
    typedef pixel<bits##T, LAYOUT >&        CS##T##_ref_t;		\
    typedef const pixel<bits##T, LAYOUT >&  CS##T##c_ref_t;		\
    typedef CS##T##_pixel_t*                CS##T##_ptr_t;		\
    typedef CS##T##c_pixel_t*               CS##T##c_ptr_t;		\
    typedef memory_based_step_iterator<CS##T##_ptr_t>			\
    CS##T##_step_ptr_t;							\
    typedef memory_based_step_iterator<CS##T##c_ptr_t>                  \
    CS##T##c_step_ptr_t;						\
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_ptr_t>> \
    CS##T##_loc_t;							\
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_ptr_t>> \
    CS##T##c_loc_t;							\
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##_step_ptr_t>> \
    CS##T##_step_loc_t;							\
    typedef memory_based_2d_locator<memory_based_step_iterator<CS##T##c_step_ptr_t>> \
    CS##T##c_step_loc_t;						\
    typedef image_view<CS##T##_loc_t>            CS##T##_view_t;	\
    typedef image_view<CS##T##c_loc_t>           CS##T##c_view_t;	\
    typedef image_view<CS##T##_step_loc_t>       CS##T##_step_view_t;	\
    typedef image_view<CS##T##c_step_loc_t>      CS##T##c_step_view_t;	\
    typedef image<CS##T##_pixel_t,false,std::allocator<unsigned char> > \
    CS##T##_image_t;

// CS = 'bgr' CS_FULL = 'rgb_t' LAYOUT='bgr_layout_t'
#define PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(T,CS,CS_FULL,LAYOUT)	\
    PSYNTH_DEFINE_BASE_TYPEDEFS_INTERNAL(T,CS,LAYOUT)			\
    typedef planar_pixel_reference<bits##T&,CS_FULL >                   \
    CS##T##_planar_ref_t;						\
    typedef planar_pixel_reference<const bits##T&,CS_FULL >             \
    CS##T##c_planar_ref_t;						\
    typedef planar_pixel_iterator<bits##T*,CS_FULL >                    \
    CS##T##_planar_ptr_t;						\
    typedef planar_pixel_iterator<const bits##T*,CS_FULL >              \
    CS##T##c_planar_ptr_t;						\
    typedef memory_based_step_iterator<CS##T##_planar_ptr_t>            \
    CS##T##_planar_step_ptr_t;						\
    typedef memory_based_step_iterator<CS##T##c_planar_ptr_t>           \
    CS##T##c_planar_step_ptr_t;						\
    typedef								\
    memory_based_2d_locator<memory_based_step_iterator<CS##T##_planar_ptr_t>> \
    CS##T##_planar_loc_t;						\
    typedef								\
    memory_based_2d_locator<memory_based_step_iterator<CS##T##c_planar_ptr_t>> \
    CS##T##c_planar_loc_t;						\
    typedef \
    memory_based_2d_locator<memory_based_step_iterator<CS##T##_planar_step_ptr_t>> \
    CS##T##_planar_step_loc_t;						\
    typedef \
    memory_based_2d_locator<memory_based_step_iterator<CS##T##c_planar_step_ptr_t>> \
    CS##T##c_planar_step_loc_t;						\
    typedef image_view<CS##T##_planar_loc_t>                            \
    CS##T##_planar_view_t;						\
    typedef image_view<CS##T##c_planar_loc_t>                           \
    CS##T##c_planar_view_t;						\
    typedef image_view<CS##T##_planar_step_loc_t>                       \
    CS##T##_planar_step_view_t;						\
    typedef image_view<CS##T##c_planar_step_loc_t>                      \
    CS##T##c_planar_step_view_t;					\
    typedef image<CS##T##_pixel_t,true,std::allocator<unsigned char> >  \
    CS##T##_planar_image_t;    

#define PSYNTH_DEFINE_BASE_TYPEDEFS(T,CS)        \
    PSYNTH_DEFINE_BASE_TYPEDEFS_INTERNAL(T,CS,CS##_layout_t)

#define PSYNTH_DEFINE_ALL_TYPEDEFS(T,CS)         \
    PSYNTH_DEFINE_ALL_TYPEDEFS_INTERNAL(T,CS,CS##_t,CS##_layout_t)

namespace boost { namespace psynth {

// forward declarations
template <typename B, typename Mn, typename Mx> struct scoped_channel_value;
struct float_zero;
struct float_one;
typedef scoped_channel_value<float,float_zero,float_one> bits32f;
typedef uint8_t  bits8;
typedef uint16_t bits16;
typedef uint32_t bits32;
typedef int8_t   bits8s;
typedef int16_t  bits16s;
typedef int32_t  bits32s;

PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8,  gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8s, gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16, gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16s,gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32 ,gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32s,gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32f,gray)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8,  bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8s, bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16, bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16s,bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32 ,bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32s,bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32f,bgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8,  argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8s, argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16, argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16s,argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32, argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32s,argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32f,argb)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8,  abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8s, abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16, abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16s,abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32 ,abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32s,abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32f,abgr)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8,  bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(8s, bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16, bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(16s,bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32 ,bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32s,bgra)
PSYNTH_SND_DEFINE_BASE_TYPEDEFS(32f,bgra)

PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8,  rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8s, rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16, rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16s,rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32 ,rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32s,rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32f,rgb)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8,  rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8s, rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16, rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16s,rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32 ,rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32s,rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32f,rgba)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8,  cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(8s, cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16, cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(16s,cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32 ,cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32s,cmyk)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS(32f,cmyk)


template <int N> struct devicen_t;
template <int N> struct devicen_layout_t;
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8,  dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8s, dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16, dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16s,dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32 ,dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32s,dev2n, devicen_t<2>, devicen_layout_t<2>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32f,dev2n, devicen_t<2>, devicen_layout_t<2>)

PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8,  dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8s, dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16, dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16s,dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32 ,dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32s,dev3n, devicen_t<3>, devicen_layout_t<3>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32f,dev3n, devicen_t<3>, devicen_layout_t<3>)

PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8,  dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8s, dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16, dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16s,dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32 ,dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32s,dev4n, devicen_t<4>, devicen_layout_t<4>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32f,dev4n, devicen_t<4>, devicen_layout_t<4>)

PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8,  dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(8s, dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16, dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(16s,dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32 ,dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32s,dev5n, devicen_t<5>, devicen_layout_t<5>)
PSYNTH_SND_DEFINE_ALL_TYPEDEFS_INTERNAL(32f,dev5n, devicen_t<5>, devicen_layout_t<5>)

} /* namespace snd */
} /* namespace psynth */

#endif
