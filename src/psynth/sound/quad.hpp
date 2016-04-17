/**
 *  Time-stamp:  <2010-11-09 02:58:53 raskolnikov>
 *
 *  @file        quad.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 18:02:01 2010
 *
 *  Quadraphonic sound.
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

#ifndef PSYNTH_SOUND_QUAD_H
#define PSYNTH_SOUND_QUAD_H

#include <cstddef>

#include <psynth/base/compat.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/stereo.hpp>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

namespace psynth
{
namespace sound
{

/**
   \addtogroup ChannelNameModel
   \{

   \brief Back Left
*/
struct back_left_channel {};

/** \brief Back Right */
struct back_right_channel {};

/** \} */

/** \ingroup ChannelSpaceModel */
typedef boost::mpl::vector4<
    left_channel,      right_channel,
    back_left_channel, back_right_channel> quad_space;

/** \ingroup LayoutModel */
typedef layout<quad_space> quad_layout;

/**
   \ingroup BufferViewConstructors
   \brief from raw quad planar data
*/
template <typename IC>
inline typename type_from_iterator<
    planar_frame_iterator<IC, quad_space> >::range
planar_quad_range (std::size_t size, IC l, IC r, IC bl, IC br)
{
    typedef typename type_from_iterator<
	planar_frame_iterator<IC, quad_space> >::range RRange;

    return RRange (size, planar_frame_iterator<IC, quad_space> (l, r, bl, br));
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_QUAD */
