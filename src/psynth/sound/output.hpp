/**
 *  Time-stamp:  <2010-11-10 13:02:37 raskolnikov>
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
#include <psynth/sound/channel_base.hpp>

namespace psynth
{
namespace sound
{

template<typename S, typename L> struct frame;
template<typename S, typename L> struct planar_frame_reference;
template<typename S, typename L> struct planar_frame_iterator;
template<typename P, typename C, typename L>
struct packed_frame;
template<typename P, bool C, typename L>
struct buffer;
template<typename P>
struct buffer_range;
template <typename B, typename C, typename L, bool M>
struct bit_aligned_frame_reference;
template <typename Ref>
struct bit_aligned_frame_iterator;

namespace detail
{

struct item_output_fn
{
    item_output_fn (std::ostream& os) : _os (os) {}

    template <class T>
    void operator () (const T& x)
    { _os << x << " ";}

private:
    std::ostream& _os;
};

template <class Frame>
std::ostream& output_frame (std::ostream& os, const Frame& f)
{
    os << "< ";
    static_for_each (f, item_output_fn (os));
    return os << ">";
}

} /* namespace detail */

template <typename S, typename L>
std::ostream& operator<< (std::ostream& os, const frame<S, L>& f)
{
    return detail::output_frame (os, f);
}

template <typename P, typename C, typename L, bool M>
std::ostream& operator<< (std::ostream& os,
			  const bit_aligned_frame_reference<P, C, L, M>& f)
{
    return detail::output_frame (os, f);
}

template <typename Ref>
std::ostream& operator<< (std::ostream& os,
			  const bit_aligned_frame_iterator<Ref>& f)
{
    os << "*";
    return detail::output_frame (os, *f);
}

template <typename P, typename C, typename L>
std::ostream& operator<< (std::ostream& os,
			  const packed_frame<P, C, L>& f)
{
    return os;
}

template <typename S, typename L>
std::ostream& operator<< (std::ostream& os,
			  const planar_frame_reference<S, L>& f)
{
    return detail::output_frame (os, f);
}


template <typename S, typename L>
std::ostream& operator<< (std::ostream& os,
			  const planar_frame_iterator<S, L>& f)
{
    os << "*";
    return detail::output_frame (os, *f);
}

template <typename F, bool P, typename A>
std::ostream& operator<< (std::ostream& os, const buffer<F, P, A>& f)
{
    os << "[";
    for (auto it = f.begin (); it != f.end (); ++it)
    {
	os << " ";
	detail::output_frame (os, *it);
    }
    return os << " ]";
}

template <typename I>
std::ostream& operator<< (std::ostream& os, const buffer_range<I>& f)
{
    os << "[";
    for (auto it = f.begin (); it != f.end (); ++it)
    {
	os << " ";
	detail::output_frame (os, *it);
    }
    return os << " ]";
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_OUTPUT_H_ */
