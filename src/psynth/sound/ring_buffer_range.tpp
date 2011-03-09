/**
 *  Time-stamp:  <2011-03-08 20:11:32 raskolnikov>
 *
 *  @file        ring_buffer.tpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Nov  9 13:57:41 2010
 *
 *  Ring buffer implementation.
 *
 *  @todo implement read/write and their with conversion counterparts
 *  with a generic function that is applied to the subrranges.
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

#ifndef PSYNTH_SOUND_RING_BUFFER_RANGE_TPP_
#define PSYNTH_SOUND_RING_BUFFER_RANGE_TPP_

#include <cassert>
#include <algorithm>

#include <psynth/sound/buffer_range_factory.hpp>

namespace psynth
{
namespace sound
{

template <class R>
template <class Position>
typename buffer_range_type<R>::type
ring_buffer_range_base<R>::sub_buffer_one (const Position& p,
					   size_type slice) const
{
    assert (slice <= avalible (r));
    if (p._pos + slice > size ())
	return sub_range (_range, p._pos, size () - p._pos);
    else
	return sub_range (_range, p._pos, slice);
}

template <class R>
template <class Position>
typename buffer_range_type<R>::type
ring_buffer_range_base<R>::sub_buffer_two (const Position& p,
					   size_type slice) const
{
    assert (slice <= avalible (p));
    
    if (p._pos + slice > size ())
	return sub_range (_range, 0, p._pos + slice - size ());
    else
	return sub_range (_range, p._pos + slice, 0);
}

template <class R>
template <class Position, class Range>
typename ring_buffer_range_base<R>::size_type
ring_buffer_range_base<R>::read (Position& r,
				 const Range& buf,
				 size_type samples) const
{
    const size_type slice = std::min (available (r), samples);
	
    if (r._pos + slice > size ())
    {
	const size_type slice_one = size () - r._pos;
	const size_type slice_two = slice - slice_one;
	copy_frames (sub_range (_range, r._pos, slice_one),
		     sub_range (buf, 0, slice_one));
	copy_frames (sub_range (_range, 0, slice_two),
		     sub_range (buf, slice_one, slice_two));
    }
    else
	copy_frames (sub_range (_range, r._pos, slice),
		     sub_range (buf, 0, slice));
	
    advance (r, slice);
    return slice;
}

template <class R>
template <class Range>
void ring_buffer_range_base<R>::write (const Range& buf, size_type nwrite)
{
    // We can use write more data than fits in the buffer. We skip the
    // first samples 'offset'.
    size_type offset = 0;
    size_type slice  = nwrite;
    
    if (slice > size ()) {
	offset = slice - size ();
	slice = size ();
    }
    
    if (_writepos._pos + slice > size ())
    {
	const size_type slice_one = size () - _writepos._pos;
	const size_type slice_two = slice - slice_one;
	copy_frames (sub_range (buf, offset, slice_one),
		     sub_range (_range, _writepos._pos, slice_one));
	copy_frames (sub_range (buf, offset + slice_one, slice_two),
		     sub_range (_range, 0, slice_two));
    } else 
	copy_frames (sub_range (buf, 0, slice),
		     sub_range (_range, _writepos._pos, slice));
	
    advance (nwrite);
}

template <class R>
template <class Position, class Range, class CC>
typename ring_buffer_range_base<R>::size_type
ring_buffer_range_base<R>::read_and_convert (Position& r,
					     const Range& buf,
					     size_type samples,
					     CC cc) const
{
    const size_type slice = std::min (available (r), samples);
	
    if (r._pos + slice > size ())
    {
	const size_type slice_one = size () - r._pos;
	const size_type slice_two = slice - slice_one;
	copy_and_convert_frames (sub_range (_range, r._pos, slice_one),
				 sub_range (buf, 0, slice_one),
				 cc);
	copy_and_convert_frames (sub_range (_range, 0, slice_two),
				 sub_range (buf, slice_one, slice_two),
				 cc);
    }
    else
	copy_and_convert_frames (sub_range (_range, r._pos, slice),
				 sub_range (buf, 0, slice),
				 cc);
	
    advance (r, slice);
    return slice;
}

template <class R>
template <class Range, class CC>
void ring_buffer_range_base<R>::write_and_convert (const Range& buf,
						   size_type nwrite, CC cc)
{
    // We can use write more data than fits in the buffer. We skip the
    // first samples 'offset'.
    size_type offset = 0;
    size_type slice = nwrite;
    if (slice > size ()) {
	offset = slice - size ();
	slice = size ();
    }
    
    if (_writepos._pos + slice > size ())
    {
	const size_type slice_one = size () - _writepos._pos;
	const size_type slice_two = slice - slice_one;
	copy_and_convert_frames (
	    sub_range (buf, offset, slice_one),
	    sub_range (_range, _writepos._pos, slice_one),
	    cc);
	copy_and_convert_frames (
	    sub_range (buf, offset + slice_one, slice_two),
	    sub_range (_range, 0, slice_two),
	    cc);
    } else 
	copy_and_convert_frames (
	    sub_range (buf, 0, slice),
	    sub_range (_range, _writepos._pos, slice),
	    cc);
	
    advance (nwrite);
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_RING_BUFFER_RANGE_TPP_ */
