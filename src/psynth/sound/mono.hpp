/**
 *  Time-stamp:  <2010-11-03 13:43:19 raskolnikov>
 *
 *  @file        mono.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 13:51:27 2010
 *
 *  Mono definitions.
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

#ifndef PSYNTH_SOUND_MONO_H
#define PSYNTH_SOUND_MONO_H

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>

#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>

namespace psynth
{
namespace sound
{

/**
  \ingroup ChannelNameModel
  \brief mono_channel
*/
struct mono_channel {};

/**
   \ingroup ChannelSpaceModel
*/
typedef boost::mpl::vector1<mono_channel> mono_space;

/**
   \ingroup LayoutModel
*/
typedef layout<mono_space> mono_layout;

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_MONO */
