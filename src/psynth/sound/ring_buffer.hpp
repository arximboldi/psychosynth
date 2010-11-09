/**
 *  Time-stamp:  <2010-11-09 17:04:56 raskolnikov>
 *
 *  @file        ring_buffer.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Nov  8 16:24:42 2010
 *
 *  Ring buffer.
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

namespace psynth
{
namespace sound
{

/**
 * Error codes for a read pointer.
 */
enum class ring_buffer_error
{
    /** No error. */
    none,

    /**
     * An underrun has ocurred. This means that there has been too much
     * data written overwritting data thas has not been previously read.
     */
    underrun,
	
    /**
     * An overrun has ocurred. This means that the reader has read more data
     * that the one actually in the buffer.
     */
    overrun
};

namespace detail
{

template<class Range, bool Mutable>
class iterator_type_from_buffer : public boost::mpl::if_<
    Mutable,
    typename Range::iterator,
    typename Range::const_iterator>::type
    
template<class Range, bool Mutable>
class iterator_traits_from_buffer : public boost::mpl::if_<
    Mutable,
    typename std::iterator_traits<Range::iterator>,
    typename std::iterator_traits<Range::const_iterator>>::type
    

/**
 * A pointer a reader can use to read data from the buffer.
 */
template <class Ring>
class ring_position
{
public:
    typedef typename Ring::size_type       size_type;
    typedef typename Ring::difference_type difference_type;
    typedef Ring                           ring_type;
    
    const size_type& offset () const
    { return _pos; }
    
private:
    friend class Ring;
    void add (difference_type n) {}
   
    size_type _pos;
};

template <class Ring>
class safe_ring_position : public ring_position<Ring, Mutable>
{
private:
    friend class Ring;
    
    void add (difference_type n)
    { _count += n; }
    
    difference_type _count;
};

template <class Position>
class ring_iterator : public boost::iterator_facade<
    ring_iterator<Position>,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::value_type,
    std::random_access_iterator_tag,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::reference_type,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::difference_type
    >
{
    typedef Position position_type;
    typedef typename Position::ring_type ring_type;

    ring_iterator () {}
    
    template <Position>
    ring_iterator (const Position& pos, ring_type* ring)
	: _position (pos)
	, _ring (ring)
    {}

    template <RingIterator>
    ring_iterator (const RingIterator& it)
	: _position (it._position)
	, _ring (it._ring)
    {}

    template <RingIterator>
    ring_iterator& operator= (const RingIterator& it)
    {
	_position = it._position;
	_ring = it._ring;
	return *this;
    }
    
    const position_type& position ()
    { return _position; }
    
private:
    void increment ()
    { _ring->increment (_position); }

    void decrement ()
    { _ring->decrement (_position); }

    void advance (std::iterator_traits<ring_iterator> n)
    { _ring->advance (_position, n); }

    std::iterator_traits<ring_iterator>::reference dereference ()
    { return _ring->at (_position); }

    template <RingIterator>
    std::iterator_traits<ring_iterator>::difference_type distance_to (
	const RingIterator& it) const
    { return _ring->distance (_position, it._position); };

    template <RingIterator>
    bool equal (const RingIterator& it) const
    { _position == it._position && _ring == it._ring };
    // TODO: Remove comparison of _ring?
    
    friend class boost::iterator_core_access;
    position_type             _position;
    position_type::ring_type* _ring;
};

} /* namespace detail */

/**
 * @note There is no const counterpart?
 */
template <typename Range, bool Mutable>
class ring_buffer_range_base
{
public:
    typedef Range range;
    
    typedef typename Range::size_type       size_type;
    typedef typename Range::difference_type difference_type;
    
    typedef ring_position<ring_buffer_range>       position;
    typedef safe_ring_position<ring_buffer_range>  safe_position;    
    
    /**
     * Constructor.
     * @param size The size of the buffer, defaults to zero.
     */
    ring_buffer_range_base ()
	: _backwards (false)
	, _startpos (0, 0)
	, _writepos (0, 0)
    {}

    /** Copy constructor */
    ring_buffer_range_base (const ring_buffer_range_base& buf)
	: _backwards (false)
	, _startpos (0, 0)
	, _writepos (0, 0)
    {}

    template<class Range2>
    explicit ring_buffer_range_base (const Range2& range);
    
    /** Copy constructor. */
    ring_buffer_range_base (const ring_buffer_range_base& buf);

    /** Copy constructor. */
    template <class Range2>
    ring_buffer_range_base (const ring_buffer_range_base<Range2>& buf);
    
    /** Assignment operator. */
    ring_buffer_range_base& operator= (ring_buffer_range_base& buf);

    /**
     * Returns the size of the buffer.
     */
    size_type size () const
    { return _buffer.size (); }

    /**
     * Returns a read pointer to the beginning of the availible data.
     */
    position begin_pos () const
    { return position (_start_pos); };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    position end_pos () const
    { return position (_write_pos); };

    /**
     * Returns a read pointer to the beginning of the availible data.
     */
    safe_position safe_begin_pos () const
    {
	return _write_pos._count > size () ?
	    safe_position (_start_pos, _write_pos._count - size ()) :
	    safe_position (_start_pos, 0);
    };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    safe_position safe_end () const
    { return _write_pos; };
    
    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    size_type availible (const safe_position& r) const
    { return _write_pos._count - r._count; }

    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    size_type availible () const
    { return _write_pos._count > size () ? size () : _write_pos._count; }
    
    /**
     * Checks a iterator for error states.
     */
    int check_position (const safe_position& reader) const
    {
	if (reader._count < _write_pos._count - _size)
	    return ring_buffer_error::underrun;
	if (reader._count > _write_pos._count)
	    return ring_buffer_error::overrun;
	return ring_buffer_error::none;
    }
    
    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader posize_ter.
     * @param buf The buffer to fill with the data.
     */
    template<class Position, class Range>
    size_t read (Position& r, Range& buf) const
    {
	return read (r, buf, buf.size ());
    };

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    template<class Position, class Range>
    size_type read (Position& r, Range& buf, size_type samples) const;

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader posize_ter.
     * @param buf The buffer to fill with the data.
     */
    template<class Position, class Range, class CC = default_channel_converter>
    size_t read_and_convert (Position& r, Range& buf, CC cc = CC ()) const
    {
	return read (r, buf, buf.size (), cc);
    };

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    template<class Position, class Range, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, Range& buf,
				size_type samples, CC cc = CC ()) const;
    
    /**
     * Write all the data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     */
    template <class Range>
    void write (const sample_buffer& buf) const
    {
	write (buf, buf.size ());
    }

    /**
     * Write some data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    template <class Range>
    void write (Range& buf, size_type samples) const;
    
    /**
     * Write all the data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     */
    template <class Range, class CC = default_channel_converter>
    void write_and_convert (const sample_buffer& buf, CC cc = CC ()) const
    {
	write (buf, buf.size (), cc);
    }

    /**
     * Write some data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    template <class Range, class CC = default_channel_converter>
    void write_and_convert (Range& buf, size_type samples, CC cc = CC ()) const;

    /**
     * Sets to zero all the contents of the buffer.
     */
    void zero ()
    {
	fill_frames (_buffer, 0); // TODO: USE SAMPLE TRAITS
    }

    /**
     * Returns wether this buffer is being written backwards.
     */
    bool backwards () const
    { return _backwards; }
    
    /**
     * Changes the reading direction of the current pointer write pointer.
     * If you are using this buffer as an intermediate buffer from another
     * medium and your where reading that medium forwards, this allows you to
     * not to lose the already read data remaining in the buffer if you
     * change the reading direction.
     * @see sync ()
     * @note Returna new range instead of modifying the current?
     */
    void set_backwards ()
    {
	_backwards = !_backwards;
	if (_writecount < _size)
	    std::swap (_startpos, _writepos);
    }
    
    /**
     * Advances a read pointer a given number of elements.
     * @param r The iterator to advance.
     * @param n The number of elements to advance.
     */
    template <typename Position> // Models PositionConcept
    void advance (Position& r, difference_type n) const
    {
	if (n >= 0)
	{
	    r._pos = (t._pos + n) % size;
	    r.add_count (n);
	}
	else
	{
	    r._pos += n;
	    while (r._pos < _buffer.begin ())
		r._pos += _size;
	    r.add_count (-n);
	}
    }

    template <typename Position>
    void increment (Position& r) const
    {
	++ r._pos;
	if (r._pos == _buffer.size)
	    r._pos = _buffer.size;
	r.add_count (1);
    }

    void decrement (Position& r) const
    {
	-- r._pos;
	if (r._pos < 0)
	    r._pos = _buffer.size - 1;
	r.add_count (-1)
    }

    /**
     * Advances the write pointer of the buffer a given number of elements.
     * @param n The number of elements to advance.
     */
    void advance (size_type n)
    {
	advance (_write_pos, n);
    }

    /**
     * Returns a new iterator in the same position but with the read count
     * calculated from the read position to the write position.
     * This function may be called to get back from an erratic iterator
     * and @b must be called whenever the write direction is changed using
     * the @c backwards () function.
     */
    safe_iterator sync (const safe_iterator& r)
    {
	if (!_backwards)
	    return safe_iterator (
		r._pos, _writecount - (r._pos <= _writepos ?
				       _writepos - r._pos :
				       _size - r._pos + _writepos));
	else
	    return safe_iterator (
		r._pos,	_writecount - (r._pos >= _writepos ?
				       r._pos - _writepos :
				       _size - _writepos + r._pos));
    }
    
private:
    bool          _backwards;  /**< @c true if we are reading and
			          writting the ringbuffer backwards. */
    size_t        _start_pos;  /**< The new starting position of the
				  ring buffer. */
    safe_position _write_pos;
    Range         _buffer;
};

template <class Range>
class ring_buffer_range : public ring_buffer_range_base<Range>
{
    typedef ring_buffer_range_base<Range> parent_type;

public:
    typedef detail::ring_iterator<parent_type::position>      iterator;
    typedef detail::ring_iterator<parent_type::safe_position> safe_iterator;

    iterator begin () const
    { return iterator (begin_pos (), this); }

    iterator end () const
    { return iterator (begin_pos (), this); }

    safe_iterator safe_begin () const
    { return safe_iterator (safe_begin_pos (), this); }

    safe_iterator safe_end () const
    { return safe_iterator (safe_begin_pos (), this); }
};

template <class Range>
struct const_ring_buffer_range :
    public ring_buffer_range_base<typename Range::const_type> {};

template <class Range>
class dynamic_ring_buffer_range : public ring_buffer_range_base<Range> {};

template <class Range>
class const_dynamic_ring_buffer_range :
    public ring_buffer_range_base<typename Range::const_type> {};

template <class Buffer>
class dynamic_ring_buffer_base
{
};

template <class Buffer>
class dynamic_ring_buffer : public ring_buffer_base<Buffer>
{
};

template <class Buffer>
class ring_buffer : ring_buffer_base<Buffer>
{    
    // TODO
};


} /* namespace sound */
} /* namespace psynth */

#include <psynth/sound/ring_buffer.tpp>

#endif /* PSYNTH_SOUND_RING_BUFFER_H_ */

