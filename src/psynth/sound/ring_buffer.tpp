/**
 *  Time-stamp:  <2010-11-09 16:50:05 raskolnikov>
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

#ifndef PSYNTH_SOUND_RING_BUFFER_TPP_
#define PSYNTH_SOUND_RING_BUFFER_TPP_

#include <cassert>

namespace psynth
{
namespace once
{

template <class R>
template <class Position, class Range>
buffer_range_type<R>::type
ring_buffer_range_base<R>::sub_buffer_one (const Position& p,
					     size_type slice)
{
    assert (slice <= avalible (r));
    if (r._pos + slice > size ())
	return sub_buffer_range (_buffer, p._pos, size () - r._pos);
    return
	return sub_buffer_range (_buffer, p._pos, slice);
}

template <class R>
template <class Position>
buffer_range_type<R>::type
ring_buffer_range_base<R>::sub_buffer_two (const Position& p,
					     size_type slice)
{
    assert (slice <= avalible (r));
    
    if (r._pos + slice > size ())
	return sub_buffer_range (_buffer, 0, r._pos + slice - size ());
    else
	return sub_buffer_range (_buffer, p._pos + slice, 0);
}

template <class R>
template <class Position, class Range>
size_type ring_buffer_range_base<R>::read (Position& r,
					     Range& buf,
					     size_type samples) const
{
    const size_type slice = min (availible (r), samples);
	
    if (position (r) + slice > size ())
    {
	const size_type slice_one = size () - r._pos;
	const size_type slice_two = r._pos + slice - size ();
	copy_frames (sub_buffer_range (_buffer, p._pos, slice_one),
		     sub_buffer_range (buf, 0, slice_one));
	copy_frames (sub_buffer_range (_buffer, 0, slice_two),
		     sub_buffer_range (buf, slice_one, slice_two));
    }
    else
	copy_frames (sub_buffer_range (_buffer, p._pos, slice),
		     sub_buffer_range (buf, 0, slice));
	
    advance (r, slice);
    return slice;
}

template <class R>
template <class Position, class Range>
void ring_buffer_range_base<R>::write (const Range& buf, size_type nwrite)
{
    // We can use write more data than fits in the buffer. We skip the
    // first samples 'offset'.
    size_type offset = 0;

    if (nwrite > _size) {
	offset = nwrite - size ();
	nwrite = size ();
    }
    
    if (_write_pos._pos + nwrite > _size)
    {
	const size_type slice_one = size () - _write_pos._pos;
	const size_type slice_two = _write_pos._pos + nwrite - size ();
	copy_frames (sub_buffer_range (buf, offset, slice_one),
		     sub_buffer_range (_buffer, _write_pos._pos, slice_one));
	copy_frames (sub_buffer_range (buf, offset + slice_one, slice_two),
		     sub_buffer_range (buf, 0, slice_two))
    } else 
	copy_frames (sub_buffer_range (buf, 0, slice),
		     sub_buffer_range (_buffer, _write_pos._pos, slice))
	
    advance (nwrite);
}

template <class R>
template <class Position, class Range, class CC>
size_type ring_buffer_range_base<R>::read_and_convert (Position& r,
					      Range& buf,
					      size_type samples,
					      CC cc) const
{
    const size_type slice = min (availible (r), samples);
	
    if (position (r) + slice > size ())
    {
	const size_type slice_one = size () - r._pos;
	const size_type slice_two = r._pos + slice - size ();
	copy_and_convert_frames (sub_buffer_range (_buffer, p._pos, slice_one),
				 sub_buffer_range (buf, 0, slice_one),
				 cc);
	copy_and_convert_frames (sub_buffer_range (_buffer, 0, slice_two),
				 sub_buffer_range (buf, slice_one, slice_two),
				 cc);
    }
    else
	copy_and_convert_frames (sub_buffer_range (_buffer, p._pos, slice),
				 sub_buffer_range (buf, 0, slice),
				 cc);
	
    advance (r, slice);
    return slice;
}

template <class R>
template <class Position, class Range, class CC>
void ring_buffer_range_base<R>::write (const Range& buf, size_type nwrite, CC cc)
{
    // We can use write more data than fits in the buffer. We skip the
    // first samples 'offset'.
    size_type offset = 0;

    if (nwrite > _size) {
	offset = nwrite - size ();
	nwrite = size ();
    }
    
    if (_write_pos._pos + nwrite > _size)
    {
	const size_type slice_one = size () - _write_pos._pos;
	const size_type slice_two = _write_pos._pos + nwrite - size ();
	copy_and_convert_frames (
	    sub_buffer_range (buf, offset, slice_one),
	    sub_buffer_range (_buffer, _write_pos._pos, slice_one),
	    cc);
	copy_and_convert_frames (
	    sub_buffer_range (buf, offset + slice_one, slice_two),
	    sub_buffer_range (buf, 0, slice_two),
	    cc);
    } else 
	copy_and_convert_frames (
	    sub_buffer_range (buf, 0, slice),
	    sub_buffer_range (_buffer, _write_pos._pos, slice),
	    cc);
	
    advance (nwrite);
}

} /* namespace once */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_RING_BUFFER_TPP_ */
