/**
 *  Time-stamp:  <2010-11-11 22:01:51 raskolnikov>
 *
 *  @file        dynamic_ring_buffer_range.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov 10 14:10:14 2010
 *
 *  Dynamic ring buffer range.
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

#ifndef PSYNTH_SOUND_DYNAMIC_RING_BUFFER_RANGE_H_
#define PSYNTH_SOUND_DYNAMIC_RING_BUFFER_RANGE_H_

#include <psynth/sound/ring_buffer_range.hpp>

#include <psynth/sound/dynamic_algorithm.hpp>
#include <psynth/sound/dynamic_buffer_range.hpp>
#include <psynth/sound/dynamic_buffer_range_factory.hpp>

namespace psynth
{
namespace sound
{

template <class Range>
class dynamic_ring_buffer_range;

template <class Range>
class dynamic_ring_buffer_range : public ring_buffer_range_base<Range>
{
    typedef ring_buffer_range_base<Range> parent_type;

public:
    typedef dynamic_ring_buffer_range<typename Range::const_range> const_range;
    
    dynamic_ring_buffer_range () {}

    dynamic_ring_buffer_range (const dynamic_ring_buffer_range& range)
	: parent_type (range) {}

    template<class Range2>
    explicit dynamic_ring_buffer_range (Range2& range)
	: parent_type (range) {}
    
    template<class Range2>
    explicit dynamic_ring_buffer_range (const Range2& range)
	: parent_type (range) {}

    template <class Range2>
    dynamic_ring_buffer_range (const dynamic_ring_buffer_range<Range2>& range)
	: parent_type (range) {}
};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_RING_BUFFER_RANGE_H_ */


