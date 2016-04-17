/**
 *  Time-stamp:  <2010-11-09 03:02:26 raskolnikov>
 *
 *  @file        surround.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 18:09:52 2010
 *
 *  Surround sound.
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

#ifndef PSYNTH_SOUND_SURROUND_H
#define PSYNTH_SOUND_SURROUND_H

#include <cstddef>
#include <boost/mpl/contains.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/quad.hpp>

namespace psynth
{
namespace sound
{

/**
   \ingroup ChannelNameModel
   \brief Center
*/
struct center_channel {};

typedef boost::mpl::vector5<
    left_channel, right_channel,
    center_channel,
    back_right_channel, back_left_channel> surround_space;

/** \ingroup LayoutModel */
typedef layout<surround_space> surround_layout;

/**
   \ingroup BufferRangeConstructors
   \brief from raw surround planar data
*/
template <typename IC> inline
typename type_from_iterator<planar_frame_iterator<IC, surround_space> >::range
planar_surround_range (std::size_t size, IC l, IC r, IC c, IC bl, IC br)
{
    typedef typename type_from_iterator<
	planar_frame_iterator<IC, surround_space> >::range RRange;
    return RRange (size, planar_frame_iterator<IC, surround_space>(
		      l, r, c, bl, br));
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_SURROUND */
