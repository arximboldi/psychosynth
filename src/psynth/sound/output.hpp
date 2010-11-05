/**
 *  Time-stamp:  <2010-11-05 12:26:03 raskolnikov>
 *
 *  @file        output.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 14:44:25 2010
 *
 *  Functions for outputting frame values...
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

#ifndef PSYNTH_SOUND_OUTPUT_H_
#define PSYNTH_SOUND_OUTPUT_H_

#include <ostream>

namespace psynth
{
namespace sound
{

template<typename S, typename L> struct frame;
template<typename S, typename L> struct planar_frame_reference;
template<typename S, typename L> struct planar_frame_iterator;
template<typename P, typename C, typename L>
struct packed_frame;
template <typename B, typename C, typename L, bool M>
struct bit_aligned_frame_reference;
template <typename Ref>
struct bit_aligned_frame_iterator;

template <typename S, typename L>
std::ostream& operator<< (std::ostream& os, const frame<S, L>& f)
{
    os << "[frame]";
    return os;
}

template <typename P, typename C, typename L, bool M>
std::ostream& operator<< (std::ostream& os,
			  const bit_aligned_frame_reference<P, C, L, M>& f)
{
    os << "[bit_aligned_frame_reference]";
    return os;
}

template <typename Ref>
std::ostream& operator<< (std::ostream& os,
			  const bit_aligned_frame_iterator<Ref>& f)
{
    os << "[bit_aligned_frame_iterator]";
    return os;
}

template <typename P, typename C, typename L>
std::ostream& operator<< (std::ostream& os,
			  const packed_frame<P, C, L>& f)
{
    os << "[packed_frame]";
    return os;
}

template <typename S, typename L>
std::ostream& operator<< (std::ostream& os,
			  const planar_frame_reference<S, L>& f)
{
    os << "[planar_frame_reference]";
    return os;
}


template <typename S, typename L>
std::ostream& operator<< (std::ostream& os,
			  const planar_frame_iterator<S, L>& f)
{
    os << "[planar_frame_iterator]";
    return os;
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_OUTPUT_H_ */
