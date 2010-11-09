/**
 *  Time-stamp:  <2010-11-09 12:43:52 raskolnikov>
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
#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/algorithm.hpp>

namespace psynth
{
namespace sound
{

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4244)
// conversion from 'sound::buffer<V,Alloc>::size_type' to 'int', possible
// loss of data (visual studio compiler doesn't realize that the two
// types are the same)
//#endif

/**
 * \ingroup BufferRangeModel BufferModel FrameBasedModel
 * \brief container interface over buffer range. Models BufferConcept,
 * FrameBasedConcept
 *
 * A 1D container whose elements are frames. It is templated over the
 * frame type, a boolean indicating whether it should be planar, and
 * an optional allocator.
 *
 * Note that its element type does not have to be a frame. \p buffer
 * can be instantiated with any Regular element, in which case it
 * models the weaker RandomAccess2DBufferConcept and does not model
 * FrameBasedConcept
 *
 * @note Should we merge buffer and range? Think about constness
 * problems.
 *
 */

template <typename Frame,
	  bool IsPlanar,
	  typename Alloc = std::allocator<unsigned char> >    
class buffer
{
public:
    typedef typename Alloc::template rebind<unsigned char>::other allocator_type;

    typedef typename range_type_from_frame<Frame, IsPlanar>::type range;
    typedef typename range::const_type  const_range;
    
    typedef typename range::size_type        size_type;
    typedef typename range::value_type       value_type;
    typedef typename range::difference_type  difference_type;
    typedef typename range::reference        reference;
    
    typedef typename range::iterator               iterator;
    typedef typename const_range::iterator         const_iterator;
    typedef typename range::reverse_iterator       reverse_iterator;
    typedef typename const_range::reverse_iterator const_reverse_iterator;
    
    #if 0
    // TODO: Is an alternative necesary or should we rely on recreate?
    explicit buffer (std::size_t alignment = 0,
		     const Alloc alloc_in  = Alloc())
	: _memory (0)
	, _align_in_bytes (alignment)
	, _alloc (alloc_in)
    {}
    #endif
    
    /* Create with size and optional initial value and
     * alignment */
    explicit buffer (size_type size = 0,
		     std::size_t alignment = 0,
	    const Alloc alloc_in = Alloc())
	: _memory (0)
	, _align_in_bytes (alignment)
	, _alloc (alloc_in)
    {
        allocate_and_default_construct (size);
    }
    
    buffer (size_type size, 
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
        allocate_and_copy (buf.size (), buf._range);
    }

    template <typename F2, bool IP2, typename Alloc2>
    buffer (const buffer<F2, IP2, Alloc2>& buf)
	: _memory(0)
	, _align_in_bytes (buf._align_in_bytes)
	, _alloc (buf._alloc)
    {
       allocate_and_copy (buf.size (), buf._range);
    }

    buffer& operator= (const buffer& buf)
    {
        if (this->size () == buf.size ())
            copy_frames (buf, _range);
        else {
            buffer tmp (buf);
            swap (tmp);
        }
        return *this;
    }

    template <typename Buf>
    buffer& operator= (const Buf& buf)
    {
        if (this->size () == buf.size ())
            copy_frames (buf, _range);
        else {
            buffer tmp (buf);
            swap (tmp);
        }
        return *this;
    }

    ~buffer()
    {
        destruct_frames (_range);
        deallocate (_range.size ());
    }

    Alloc&       allocator () { return _alloc; }
    Alloc const& allocator () const { return _alloc; }

    size_type size () const
    {
	return _range.size ();
    }
    
    void swap (buffer& buf)
    {
        // required by MutableContainerConcept
        using std::swap;
        swap (_align_in_bytes, buf._align_in_bytes);
        swap (_memory,         buf._memory);
        swap (_range,          buf._range); 
        swap (_alloc,          buf._alloc);
    }    

    void recreate (size_type size,
		   std::size_t alignment=0,
		   const Alloc alloc_in = Alloc ())
    {
        if (size != _range.size () ||
	    _align_in_bytes != alignment ||
	    alloc_in != _alloc)
	{
            buffer tmp (size, alignment, alloc_in);
            swap (tmp);
        }
    }
    
    void recreate (size_type size, 
		   const Frame& frame_in,
		   std::size_t alignment,
		   const Alloc alloc_in = Alloc())
    {
        if (size != size() ||
	    _align_in_bytes != alignment ||
	    alloc_in != _alloc)
	{
            buffer tmp (size, frame_in, alignment, alloc_in);
            swap (tmp);
        }
    }

    iterator frames () 
    { return _range.frames (); }

    const_iterator frames () const 
    { return _range.frames (); }
    
    iterator begin ()
    { return _range.begin (); }
    
    iterator end ()
    { return _range.end (); } // potential performance problem!

    const_iterator begin () const
    { return _range.begin (); }
    
    const_iterator end () const
    { return _range.end (); } // potential performance problem!
    
    reverse_iterator rbegin ()
    { return _range.rbegin (); }
    
    reverse_iterator rend ()
    { return _range.rend (); }
    
    const_reverse_iterator rbegin () const
    { return _range.rbegin (); }
    
    const_reverse_iterator rend () const
    { return _range.rend (); }
    
    reference operator [] (difference_type i) const
    {
	return begin() [i];
    } // potential performance problem!

    iterator at (difference_type i) const
    {
	return begin () + i;
    }
    //\}@
    
private:
    template <typename F, bool P, typename A> friend 
    const typename buffer<F, P, A>::range&
    range (buffer<F, P, A>& buf);

    template <typename F, bool P, typename A> friend 
    const typename buffer<F, P, A>::const_range
    const_range (const buffer<F, P, A>& buf);
    
    unsigned char* _memory;
    std::size_t    _align_in_bytes;
    allocator_type _alloc;
    range          _range;
    
    void allocate_and_default_construct (size_type size)
    { 
        try {
            allocate_ (size, boost::mpl::bool_<IsPlanar>());
            default_construct_frames (_range);
        } catch (...) {
	    deallocate (size);
	    throw;
	}
    }

    void allocate_and_fill (size_type size, const Frame& frame_in)
    {
        try {
            allocate_ (size, boost::mpl::bool_<IsPlanar>());
            uninitialized_fill_frames (_range, frame_in);
        } catch(...) {
	    deallocate(size);
	    throw;
	}
    }

    template <typename Range>
    void allocate_and_copy (size_type size, const Range& v)
    { 
        try {
            allocate_ (size, boost::mpl::bool_<IsPlanar>());
            uninitialized_copy_frames (v, _range);
        } catch(...) {
	    deallocate(size);
	    throw;
	}
    }

    void deallocate (size_type size)
    { 
        if (_memory)
	    _alloc.deallocate (_memory, total_allocated_size_in_bytes (size));
    }

    std::size_t total_allocated_size_in_bytes (size_type size) const
    {
        std::size_t size_in_units = size * memunit_step (
	    typename range::iterator ());
        if (IsPlanar)
            size_in_units = size_in_units * num_samples<range>::value;
	
        // return the size rounded up to the nearest byte
        return (size_in_units +
		byte_to_memunit<typename range::iterator>::value - 1) /
	    byte_to_memunit<typename range::iterator>::value
            + (_align_in_bytes > 0 ? _align_in_bytes - 1 : 0);
	// add extra padding in case we need to align the first buffer frame
    }

    std::size_t get_size_in_memunits (size_type size) const
    {
	// number of units per row
        size_type size_in_memunits =
	    size * memunit_step (typename range::iterator ());
        if (_align_in_bytes > 0) {
            std::size_t alignment_in_memunits =
		_align_in_bytes *
		byte_to_memunit<typename range::iterator>::value;
            return align (size_in_memunits, alignment_in_memunits);
        }
        return size_in_memunits;
    }
    
    void allocate_ (const size_type& size, boost::mpl::false_)
    {
	// if it throws and _memory!=0 the client must deallocate _memory
        _memory = _alloc.allocate (total_allocated_size_in_bytes (size));

	unsigned char* tmp = (_align_in_bytes > 0) ?
	    (unsigned char*) align ((std::size_t) _memory, _align_in_bytes) :
	    _memory;
	
        _range = range (size, typename range::iterator (tmp));
    }

    void allocate_ (size_type size, boost::mpl::true_)
    {
        // if it throws and _memory!=0 the client must deallocate _memory
        std::size_t plane_size = get_size_in_memunits (size);
        _memory = _alloc.allocate (total_allocated_size_in_bytes (size));
	
        unsigned char* tmp = (_align_in_bytes > 0) ?
	    (unsigned char*) align ((std::size_t)_memory, _align_in_bytes) :
	    _memory;
	
        typename range::iterator first; 
        for (int i = 0; i < num_samples<range>::value; ++i)
	{
            dynamic_at_c (first, i) = (typename sample_type<range>::type *) tmp;
            memunit_advance (dynamic_at_c (first, i), plane_size * i);
        }
	
        _range = range (size, first);
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
    if (const_range (buf1).size() != const_range (buf2).size())
	return false;
    return equal_frames (const_range (buf1), const_range (buf2));
}

template <typename Frame1, bool IsPlanar1, typename Alloc1,
	  typename Frame2, bool IsPlanar2, typename Alloc2>
bool operator != (const buffer<Frame1, IsPlanar1, Alloc1>& buf1,
		  const buffer<Frame2, IsPlanar2, Alloc2>& buf2)
{
    return !(buf1 == buf2);
}

/**@{
   \name range, const_range
   \brief Get an buffer range from an buffer

   \ingroup BufferModel

   \brief Returns the non-constant-frame range of an buffer
*/
template <typename Frame, bool IsPlanar, typename Alloc> inline 
const typename buffer<Frame, IsPlanar, Alloc>::range&
range (buffer<Frame, IsPlanar, Alloc>& buf)
{
    return buf._range;
}

/**
   \brief Returns the constant-frame range of an buffer
*/
template <typename Frame, bool IsPlanar, typename Alloc> inline 
const typename buffer <Frame, IsPlanar, Alloc>::const_range
const_range (const buffer<Frame,IsPlanar,Alloc>& buf)
{ 
    return static_cast<
	const typename buffer<Frame, IsPlanar, Alloc>::const_range> (buf._range); 
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
    public boost::mpl::bool_<IsPlanar> {};

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_BUFFER */
