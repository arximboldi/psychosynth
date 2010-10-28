/**
 *  Time-stamp:  <2010-10-28 14:28:24 raskolnikov>
 *
 *  @file        buffer.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 21 11:56:59 2010
 *
 *  Sound Buffer implementation.
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

#ifndef PSYNTH_SOUND_BUFFER_H
#define PSYNTH_SOUND_BUFFER_H

#include <cstddef>
#include <memory>
#include <psynth/base/compat.hpp>
#include <psynth/sound/buffer_view.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/algorithm.hpp>

namespace psynth
{
namespace sound
{

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4244)
// conversion from 'sound::buffer<V,Alloc>::coord_t' to 'int', possible
// loss of data (visual studio compiler doesn't realize that the two
// types are the same)
//#endif

/**
   \ingroup BufferModel FrameBasedModel
   \brief container interface over buffer view. Models BufferConcept,
   FrameBasedConcept
 
   A 1D container whose elements are frames. It is templated over the
   frame type, a boolean indicating whether it should be planar, and
   an optional allocator.

   Note that its element type does not have to be a frame. \p buffer
   can be instantiated with any Regular element, in which case it
   models the weaker RandomAccess2DBufferConcept and does not model
   FrameBasedConcept
*/

template <typename Frame,
	  bool IsPlanar,
	  typename Alloc = std::allocator<unsigned char> >    
class buffer
{
public:
    typedef typename Alloc::template rebind<unsigned char>::other allocator_type;
    typedef typename view_type_from_frame<Frame, IsPlanar>::type view_t;

    typedef typename view_t::const_t     const_view_t;
    typedef typename view_t::coord_t     coord_t;
    typedef typename view_t::value_type  value_type;
    
    const coord_t& size () const
    {
	return _view.size ();
    }

    explicit buffer (std::size_t alignment = 0,
		     const Alloc alloc_in  = Alloc())
	: _memory (0)
	, _align_in_bytes (alignment)
	, _alloc (alloc_in)
    {}

    /* Create with size and optional initial value and
     * alignment */
    buffer (coord_t& size,
	    std::size_t alignment = 0,
	    const Alloc alloc_in = Alloc())
	: _memory (0)
	, _align_in_bytes (alignment)
	, _alloc (alloc_in)
    {
        allocate_and_default_construct (size);
    }
    
    buffer (const coord_t& size, 
	    const Frame& frame_in,
	    std::size_t alignment,
	    const Alloc alloc_in = Alloc())
	: _memory (0)
	, _align_in_bytes (alignment)
	, _alloc (alloc_in)
    {
        allocate_and_fill (size, frame_in);
    }
    
    buffer (const buffer& buf)
	: _memory(0)
	, _align_in_bytes (buf._align_in_bytes)
	, _alloc(buf._alloc)
    {
        allocate_and_copy (buf.size (), buf._view);
    }

    template <typename F2, bool IP2, typename Alloc2>
    buffer (const buffer<F2, IP2, Alloc2>& buf)
	: _memory(0)
	, _align_in_bytes (buf._align_in_bytes)
	, _alloc (buf._alloc)
    {
       allocate_and_copy (buf.size (), buf._view);
    }

    buffer& operator= (const buffer& buf)
    {
        if (size () == buf.size ())
            copy_frames (buf._view, _view);
        else {
            buffer tmp (buf);
            swap (tmp);
        }
        return *this;
    }

    template <typename Buf>
    buffer& operator= (const Buf& buf)
    {
        if (size () == buf.size ())
            copy_frames (buf._view, _view);
        else {
            buffer tmp (buf);
            swap (tmp);
        }
        return *this;
    }

    ~buffer()
    {
        destruct_frames (_view);
        deallocate (_view.size ());
    }

    Alloc&       allocator () { return _alloc; }
    Alloc const& allocator () const { return _alloc; }

    void swap (buffer& buf)
    {
        // required by MutableContainerConcept
        using std::swap;
        swap (_align_in_bytes, buf._align_in_bytes);
        swap (_memory,         buf._memory);
        swap (_view,           buf._view); 
        swap (_alloc,          buf._alloc);
    }    

    void recreate (const coord_t& size,
		   std::size_t alignment=0,
		   const Alloc alloc_in = Alloc ())
    {
        if (size != _view.size() ||
	    _align_in_bytes != alignment ||
	    alloc_in != _alloc)
	{
            buffer tmp (dims, alignment, alloc_in);
            swap (tmp);
        }
    }
    
    void recreate (const coord_t& size, 
		   const Frame& frame_in,
		   std::size_t alignment,
		   const Alloc alloc_in = Alloc())
    {
        if (dims != _view.size() ||
	    _align_in_bytes != alignment ||
	    alloc_in != _alloc)
	{
            buffer tmp (size, frame_in, alignment, alloc_in);
            swap (tmp);
        }
    }
    
private:
    view_t         _view;
    // contains pointer to the frames, the buffer size and ways to
    // navigate frames
    
    unsigned char* _memory;
    std::size_t    _align_in_bytes;
    allocator_type _alloc;

    void allocate_and_default_construct (const point_t& size)
    { 
        try {
            allocate_ (size, mpl::bool_<IsPlanar>());
            default_construct_frames (_view);
        } catch (...) {
	    deallocate (size);
	    throw;
	}
    }

    void allocate_and_fill (const point_t& size, const Frame& frame_in)
    {
        try {
            allocate_ (size, mpl::bool_<IsPlanar>());
            uninitialized_fill_frames (_view, frame_in);
        } catch(...) {
	    deallocate(size);
	    throw;
	}
    }

    template <typename View>
    void allocate_and_copy (const point_t& size, const View& v)
    { 
        try {
            allocate_ (size, mpl::bool_<IsPlanar>());
            uninitialized_copy_frames (v, _view);
        } catch(...) {
	    deallocate(size);
	    throw;
	}
    }

    void deallocate (const point_t& size)
    { 
        if (_memory)
	    _alloc.deallocate (_memory, total_allocated_size_in_bytes (size));
    }

    std::size_t total_allocated_size_in_bytes (const point_t& size) const
    {
        std::size_t size_in_units = size ()
        if (IsPlanar)
            size_in_units = size_in_units * num_samples<view_t>::value;
	
        // return the size rounded up to the nearest byte
        return (size_in_units +
		byte_to_memunit<typename view_t::x_iterator>::value - 1) /
	    byte_to_memunit<typename view_t::x_iterator>::value
            + (_align_in_bytes > 0 ? _align_in_bytes - 1 : 0);
	// add extra padding in case we need to align the first buffer frame
    }

    std::size_t get_row_size_in_memunits (x_coord_t width) const
    {
	// number of units per row
        std::size_t size_in_memunits =
	    width * memunit_step (typename view_t::x_iterator ());
        if (_align_in_bytes > 0) {
            std::size_t alignment_in_memunits =
		_align_in_bytes *
		byte_to_memunit<typename view_t::x_iterator>::value;
            return align (size_in_memunits, alignment_in_memunits);
        }
        return size_in_memunits;
    }
    
    void allocate_ (const coord_t& size, mpl::false_)
    {
	// if it throws and _memory!=0 the client must deallocate _memory
        _memory = _alloc.allocate (total_allocated_size_in_bytes (size));

	unsigned char* tmp = (_align_in_bytes > 0) ?
	    (unsigned char*) align ((std::size_t) _memory, _align_in_bytes) :
	    _memory;
        _view = view_t (size, typename view_t::locator (
			    typename view_t::x_iterator (tmp),
			    get_row_size_in_memunits (size.x)));
    }

    void allocate_ (const point_t& size, mpl::true_)
    {
        // if it throws and _memory!=0 the client must deallocate _memory
        std::size_t row_size = get_row_size_in_memunits (size.x);
        std::size_t plane_size = row_size * size.y;
        _memory = _alloc.allocate (total_allocated_size_in_bytes (size));
	
        unsigned char* tmp = (_align_in_bytes > 0) ?
	    (unsigned char*) align ((std::size_t)_memory, _align_in_bytes) :
	    _memory;
	
        typename view_t::x_iterator first; 
        for (int i = 0; i < num_samples<view_t>::value; ++i)
	{
            dynamic_at_c (first, i) = (typename sample_type<view_t>::type *) tmp;
            memunit_advance (dynamic_at_c (first, i), plane_size * i);
        }
        _view = view_t (size, typename view_t::locator (first, row_size));
    }
};

template <typename Frame, bool IsPlanar, typename Alloc>
void swap (buffer<Frame, IsPlanar, Alloc>& buf1,
	   buffer<Frame, IsPlanar, Alloc>& buf2)
{
    buf1.swap (buf2); 
}

template <typename Frame1, bool IsPlanar1, typename Alloc1,
	  typename Frame2, bool IsPlanar2, typename Alloc2>
bool operator == (const buffer<Frame1, IsPlanar1, Alloc1>& buf1,
		  const buffer<Frame2, IsPlanar2, Alloc2>& buf2)
{
    if ((void*)(&buf1) == (void*)(&buf2))
	return true;
    if (const_view(buf1).size() != const_view (buf2).size())
	return false;
    return equal_frames (const_view (buf1), const_view (buf2));
}

template <typename Frame1, bool IsPlanar1, typename Alloc1,
	  typename Frame2, bool IsPlanar2, typename Alloc2>
bool operator != (const buffer<Frame1, IsPlanar1, Alloc1>& buf1,
		  const buffer<Frame2, IsPlanar2, Alloc2>& im2)
{
    return !(im1==im2);
}

/**@{
   \name view, const_view
   \brief Get an buffer view from an buffer

   \ingroup BufferModel

   \brief Returns the non-constant-frame view of an buffer
*/
template <typename Frame, bool IsPlanar, typename Alloc> inline 
const typename buffer<Frame, IsPlanar, Alloc>::view_t&
view (buffer<Frame, IsPlanar, Alloc>& buf)
{
    return buf._view;
}

/**
   \brief Returns the constant-frame view of an buffer
*/
template <typename Frame, bool IsPlanar, typename Alloc> inline 
const typename buffer <Frame, IsPlanar, Alloc>::const_view_t
const_view(const buffer<Frame,IsPlanar,Alloc>& buf)
{ 
    return static_cast<
	const typename buffer<Frame,IsPlanar,Alloc>::const_view_t> (buf._view); 
}
/** @} */

/*
 *
 *  FrameBasedConcept
 *
 */

template <typename Frame, bool IsPlanar, typename Alloc>
struct sample_type<buffer<Frame,IsPlanar,Alloc> > :
    public sample_type<Frame> {}; 

template <typename Frame, bool IsPlanar, typename Alloc>
struct channel_space_type<buffer<Frame,IsPlanar,Alloc> >  :
    public channel_space_type<Frame> {};

template <typename Frame, bool IsPlanar, typename Alloc>
struct sample_mapping_type<buffer<Frame,IsPlanar,Alloc> > :
    public sample_mapping_type<Frame> {};

template <typename Frame, bool IsPlanar, typename Alloc>
struct is_planar<buffer<Frame,IsPlanar,Alloc> > :
    public mpl::bool_<IsPlanar> {};

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_BUFFER */
