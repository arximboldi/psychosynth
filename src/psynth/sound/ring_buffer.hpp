/**
 *  Time-stamp:  <2011-03-08 18:37:43 raskolnikov>
 *
 *  @file        ring_buffer.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Nov  9 17:07:56 2010
 *
 *  Ring buffers that hold their own data for convenience.
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

#ifndef PSYNTH_SOUND_RING_BUFFER_H_
#define PSYNTH_SOUND_RING_BUFFER_H_

#include <psynth/sound/buffer.hpp>
#include <psynth/sound/ring_buffer_range.hpp>

namespace psynth
{
namespace sound
{

/**
 * @todo Reset on recreate.
 */
template <class Buffer, template <class> class BRange>
class ring_buffer_base
{
public:
    typedef BRange<typename Buffer::range> range_base;
    typedef BRange<typename Buffer::const_range> const_range_base;

    typedef typename range_base::difference_type difference_type;
    typedef typename range_base::size_type       size_type;
    typedef typename range_base::position        position;
    typedef typename range_base::safe_position   safe_position;
    
    /* Create with size and optional initial value and
     * alignment */

    explicit ring_buffer_base (size_type size = 0,
			       std::size_t alignment = 0)
	: _buffer (size, alignment)
	, _range  (range_base (range (_buffer)))
    {}

    template <typename Allocator>
    explicit ring_buffer_base (size_type size,
			       std::size_t alignment,
			       Allocator alloc_in)
	: _buffer (size, alignment, alloc_in)
	, _range  (range_base (range (_buffer)))
    {}
    
    explicit ring_buffer_base (const Buffer& buf)
	: _buffer (buf)
	, _range (range_base (range (_buffer)))
    {
	_range.advance (size ());
    }

    template <typename F2, bool IP2, typename Alloc2>
    explicit ring_buffer_base (const buffer<F2, IP2, Alloc2>& buf)
	: _buffer (buf)
	, _range (range_base (range (_buffer)))
    {
	_range.advance (size ());
    }

    ring_buffer_base& operator= (const ring_buffer_base& buf)
    {
	_buffer = buf._buffer;
	_range  = range_base (range (_buffer));
	_range.advance (buf._range.count ());
        return *this;
    }

    template <class Buffer2, template<class>class BRange2>
    ring_buffer_base& operator= (const ring_buffer_base<Buffer2, BRange2>& buf)
    {
	_buffer = buf._buffer;
	_range  = range_base (range (_buffer));
	_range.advance (buf._range.count ());
        return *this;
    }

    size_type size () const
    { return _range.size (); }

    void swap (ring_buffer_base& buf)
    {
        // required by MutableContainerConcept
        using std::swap;
        swap (_buffer, buf._buffer);
        swap (_range,  buf._range);
    }

    
    void recreate (size_type size,
		   std::size_t alignment = 0)
    {
	_buffer.recreate (size, alignment);
	_range = range_base (range (_buffer));
    }

    template <typename Allocator>
    void recreate (size_type size,
		   std::size_t alignment,
		   const Allocator alloc_in)
    {
	_buffer.recreate (size, alignment, alloc_in);
	_range = range_base (range (_buffer));
    }

    
#ifdef PSYNTH_BUFFER_MODEL_RANGE

    position begin_pos () const
    { return _range.begin_pos (); }

    position end_pos () const
    { return _range.end_pos (); }

    safe_position safe_begin_pos () const
    { return _range.safe_begin_pos (); }

    safe_position safe_end_pos () const
    { return _range.safe_end_pos (); }

    /** @see range_buffer_base::available */
    size_type available (const safe_position& r) const
    { return _range.available (r); }

    /** @see range_buffer_base::available */
    size_type available () const
    { return _range.available (); }

    /** @see range_buffer_base::check_position */
    ring_buffer_error check_position (const safe_position& r) const
    { return _range.check_position (r); }

    /** @see range_buffer_base::read */
    template<class Position, class Range>
    size_type read (Position& r, Range& buf) const
    { return _range.read (r, buf, buf.size ()); };
    
    /** @see range_buffer_base::read */
    template<class Position, class Range>
    size_type read (Position& r, Range& buf, size_type samples) const
    { return _range.read (r, buf, samples); }

    /** @see range_buffer_base::read_and_convert */
    template<class Position, class Range, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, const Range& buf, CC cc = CC ()) const
    { return _range.read_and_convert (r, buf, buf.size (), cc); };

    /** @see range_buffer_base::read_and_convert */
    template<class Position, class Range, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, const Range& buf,
				size_type samples, CC cc = CC ()) const
    { return _range.read_and_convert (r, buf, samples, cc); }

    /** @see range_buffer_base::read_and_convert */
    template<class Position, class Range, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, Range& buf, CC cc = CC ()) const
    { return _range.read_and_convert (r, buf, buf.size (), cc); };

    /** @see range_buffer_base::read_and_convert */
    template<class Position, class Range, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, Range& buf,
				size_type samples, CC cc = CC ()) const
    { return _range.read_and_convert (r, buf, samples, cc); }
    
    /** @see range_buffer_base::write */
    template <class Range>
    void write (const Range& buf)
    { return _range.write (buf, buf.size ()); }

    /** @see range_buffer_base::write */
    template <class Range>
    void write (const Range& buf, size_type samples)
    { _range.write (buf, samples); }
    
    /** @see range_buffer_base::write_and_convert */
    template <class Range, class CC = default_channel_converter>
    void write_and_convert (const Range& buf, CC cc = CC ())
    { _range.write_and_convert (buf, buf.size (), cc); }
    
    /** @see range_buffer_base::write_and_convert */
    template <class Range, class CC = default_channel_converter>
    void write_and_convert (const Range& buf, size_type samples, CC cc = CC ())
    { _range.write_and_convert (buf, samples, cc); }

    /** @see range_buffer_base::zero */
    void zero ()
    { _range.zero (); }

    /** @see range_buffer_base::backwards */
    bool is_backwards () const
    { return _range.is_backwards (); }
    
    /** @see range_buffer_base::set_backwards */
    void set_backwards ()
    { return _range.set_backwards (); }

    template <typename Position> // Models PositionConcept
    void advance (Position& r, difference_type n) const
    { return _range.advance (r, n); }

    template <typename Position>
    void increment (Position& r) const
    { return _range.increment (r); }

    template <typename Position>
    void decrement (Position& r) const
    { return _range.decrement (r); }

    void advance (size_type n)
    { return _range.advance (n); }

    difference_type count () const
    { return _range.count (); }
    
    safe_position sync (const safe_position& r) const
    { return _range.sync (); }

#endif /* PSYNTH_BUFFER_MODEL_RANGE */
    
protected:
    Buffer      _buffer;
    range_base  _range;
};

template <class Buffer>
class ring_buffer : public ring_buffer_base<Buffer, ring_buffer_range>
{
    typedef ring_buffer_base<Buffer, ring_buffer_range> parent_type;

public:
    typedef typename Buffer::allocator_type allocator_type;
    
    typedef typename parent_type::range_base       range;
    typedef typename parent_type::const_range_base const_range;
    
    typedef typename range::value_type          value_type;
    typedef typename range::reference           reference;
    
    typedef typename range::iterator            iterator;
    typedef typename const_range::iterator      const_iterator;
    typedef typename range::safe_iterator       safe_iterator;
    typedef typename const_range::safe_iterator const_safe_iterator;

    /**
     * Create with size and optional initial value and
     * alignment.
     * @todo When will GCC support inheriting constructors? :(
     */
    explicit ring_buffer (typename parent_type::size_type size = 0,
			  std::size_t alignment = 0,
			  const typename Buffer::allocator_type
			  alloc_in = typename Buffer::allocator_type ())
	: parent_type (size, alignment, alloc_in)
    {}
    
    explicit ring_buffer (const Buffer& buf)
	: parent_type (buf)
    {}

    template <typename F2, bool IP2, typename Alloc2>
    explicit ring_buffer (const buffer<F2, IP2, Alloc2>& buf)
	: parent_type (buf)
    {
    }

    ring_buffer& operator= (const ring_buffer& buf)
    {
	parent_type::operator= (buf);
        return *this;
    }

    template <class Buffer2>
    ring_buffer& operator= (const ring_buffer<Buffer2>& buf)
    {
	parent_type::operator= (buf);
        return *this;
    }


#ifdef PSYNTH_BUFFER_MODELS_RANGE
    
    iterator begin ()
    { return this->_range.begin (); }

    iterator end ()
    { return this->_range.end (); }

    safe_iterator safe_begin ()
    { return this->_range.safe_begin (); }

    safe_iterator safe_end ()
    { return this->_range.safe_end (); }

    const_iterator begin () const
    { return this->_range.begin (); }

    const_iterator end () const
    { return this->_range.end (); }

    const_safe_iterator safe_begin () const
    { return this->_range.safe_begin (); }

    const_safe_iterator safe_end () const
    { return this->_range.safe_end (); }    

#endif /* PSYNTH_BUFFER_MODEL_RANGE */
    
    allocator_type&       allocator ()
    { return this->_buffer.allocator (); }

    allocator_type const& allocator () const
    { return this->_buffer.allocator (); }
};

/*
 *
 *  @todo FrameBasedConcept
 *  
 */

/*
 *
 *  @todo DynamicStepTypeConcept
 *  
 */
    
} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_RING_BUFFER_H_ */

