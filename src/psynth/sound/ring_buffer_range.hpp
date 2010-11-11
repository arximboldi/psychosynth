/**
 *  Time-stamp:  <2010-11-11 23:07:02 raskolnikov>
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

#ifndef PSYNTH_SOUND_RING_BUFFER_RANGE_H_
#define PSYNTH_SOUND_RING_BUFFER_RANGE_H_

#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/buffer_range.hpp>

namespace psynth
{
namespace sound
{

class default_channel_converter;

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

    ring_position () {}
    ring_position (const ring_position& r) : _pos (r._pos) {}
    ring_position (size_type p) : _pos (p) {}
    ring_position& operator= (const ring_position& p)
    { _pos = p._pos; }

    size_type offset () const
    { return _pos; }

    bool operator== (const ring_position& p) const
    { return  _pos == p._pos; }
    
    // TODO: Make private
    void _add (difference_type n) {}
    size_type _pos;
};

template <class Ring>
class safe_ring_position : public ring_position<Ring>
{
    typedef ring_position<Ring> parent_type;

public:
    typedef typename parent_type::size_type size_type;
    typedef typename parent_type::difference_type difference_type;
    
    safe_ring_position () {}
    safe_ring_position (const safe_ring_position& r)
	: parent_type (r), _count (r._count) {}
    safe_ring_position (size_type p, size_type c)
	: parent_type (p), _count (c) {}
    
    safe_ring_position& operator= (const safe_ring_position& p)
    { parent_type::opertor= (p); _count = p._count; return *this; }

    difference_type count () const { return _count; }

    bool operator== (const safe_ring_position& p) const
    { return parent_type::operator== (p) && _count == p._count; }
    
    // TODO: Make private
    void _add (difference_type n)
    { _count += n; }
    
    difference_type _count;
};

template <class Position, class Ring>
class ring_iterator : public boost::iterator_facade<
    ring_iterator<Position, Ring>,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::value_type,
    std::random_access_iterator_tag,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::reference,
    typename std::iterator_traits<
	typename Position::ring_type::range::iterator>::difference_type
    >
{
public:
    typedef Position position_type;
    typedef Ring     ring_type;

    ring_iterator () {}
    
    ring_iterator (const Position& pos, const ring_type* ring)
	: _position (pos)
	, _ring (ring)
    {}

    ring_iterator (const ring_iterator& it)
	: _position (it._position)
	, _ring (it._ring)
    {}

    template <typename RingIterator>
    ring_iterator (const RingIterator& it)
	: _position (it._position)
	, _ring (it._ring)
    {}

    ring_iterator& operator= (const ring_iterator& it)
    {
	_position = it._position;
	_ring = it._ring;
	return *this;
    }
    
    template <typename RingIterator>
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

    void advance (typename std::iterator_traits<ring_iterator>::difference_type n)
    { _ring->advance (_position, n); }

    typename std::iterator_traits<ring_iterator>::reference dereference () const
    { return (*_ring) [_position._pos]; }

    template <class RingIterator>
    typename std::iterator_traits<ring_iterator>::difference_type distance_to (
	const RingIterator& it) const
    { return _ring->distance (_position, it._position); };

    template <typename RingIterator>
    bool equal (const RingIterator& it) const
    { return _position == it._position && _ring == it._ring; };
    // TODO: Remove comparison of _ring?
    
    friend class boost::iterator_core_access;
    position_type      _position;
    const ring_type*   _ring;
};

} /* namespace detail */

/**
 * @note There is no const counterpart?
 */
template <typename Range>
class ring_buffer_range_base
{
public:
    typedef Range range;
    
    typedef typename Range::size_type       size_type;
    typedef typename Range::difference_type difference_type;
    
    typedef detail::ring_position<ring_buffer_range_base>       position;
    typedef detail::safe_ring_position<ring_buffer_range_base>  safe_position;    
    
    /**
     * Constructor.
     * @param size The size of the buffer, defaults to zero.
     */
    ring_buffer_range_base ()
	: _backwards (false)
	, _startpos (0)
	, _writepos (0, 0)
    {}

    /** Copy constructor */
    ring_buffer_range_base (const ring_buffer_range_base& range)
	: _backwards (range._backwards)
	, _startpos (0)
	, _writepos (range._writepos)
	, _range (range._range)
    {}

    template<class Range2>
    explicit ring_buffer_range_base (Range2& range)
	: _backwards (false)
	, _startpos (0)
	, _writepos (0, 0)
	, _range (range)
    {}
    
    template<class Range2>
    explicit ring_buffer_range_base (const Range2& range)
	: _backwards (false)
	, _startpos (0)
	, _writepos (0, 0)
	, _range (range)
    {}
    
    /** Copy constructor. */
    template <class Range2>
    ring_buffer_range_base (const ring_buffer_range_base<Range2>& range)
	: _backwards (range._backwards)
	, _startpos (0)
	, _writepos (range._writepos)
	, _range (range._range)
    {}
    
    /** Assignment operator. */
    ring_buffer_range_base& operator= (ring_buffer_range_base& range)
    {
	_backwards = range._backwards;
	_startpos  = range._startpos;
	_writepos  = range._writepos;
	return *this;
    }

    /**
     * Returns the size of the buffer.
     */
    size_type size () const
    { return _range.size (); }

    /**
     * Returns a read pointer to the beginning of the availible data.
     */
    position begin_pos () const
    { return position (_startpos); };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    position end_pos () const
    { return position (_writepos); };

    /**
     * Returns a read pointer to the beginning of the availible data.
     */
    safe_position safe_begin_pos () const
    {
	return _writepos._count > (difference_type) size () ?
	    safe_position (_startpos, _writepos._count - size ()) :
	    safe_position (_startpos, 0);
    };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    safe_position safe_end_pos () const
    { return _writepos; };
    
    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    size_type availible (const safe_position& r) const
    { return _writepos._count - r._count; }

    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    size_type availible (const position& r) const
    {
	return _writepos._pos > r._pos ?
	    _writepos._pos - r._pos :
	    _writepos._pos + size () - r._pos;
    }
    
    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    size_type availible () const
    { return _writepos._count > size () ? size () : _writepos._count; }
    
    /**
     * Checks a iterator for error states.
     */
    int check_position (const safe_position& reader) const
    {
	if (reader._count < _writepos._count - size ())
	    return ring_buffer_error::underrun;
	if (reader._count > _writepos._count)
	    return ring_buffer_error::overrun;
	return ring_buffer_error::none;
    }

    template <class Position>
    typename buffer_range_type<Range>::type
    sub_buffer_one (const Position& p, size_type slice) const;

    template <class Position>
    typename buffer_range_type<Range>::type
    sub_buffer_two (const Position& p, size_type slice) const;
    
    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader posize_ter.
     * @param buf The buffer to fill with the data.
     */
    template<class Position, class Range2>
    size_t read (Position& r, Range2& range) const
    {
	return read (r, range, range.size ());
    };

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    template<class Position, class Range2>
    size_type read (Position& r, Range2& range, size_type samples) const;

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader posize_ter.
     * @param buf The buffer to fill with the data.
     */
    template<class Position, class Range2, class CC = default_channel_converter>
    size_t read_and_convert (Position& r, Range2& range, CC cc = CC ()) const
    {
	return read (r, range, range.size (), cc);
    };

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    template<class Position, class Range2, class CC = default_channel_converter>
    size_type read_and_convert (Position& r, Range2& range,
				size_type samples, CC cc = CC ()) const;
    
    /**
     * Write all the data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     */
    template <class Range2>
    void write (const Range2& range)
    {
	write (range, range.size ());
    }

    /**
     * Write some data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    template <class Range2>
    void write (const Range2& range, size_type samples);
    
    /**
     * Write all the data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     */
    template <class Range2, class CC = default_channel_converter>
    void write_and_convert (const Range2& range, CC cc = CC ())
    {
	write (range, range.size (), cc);
    }

    /**
     * Write some data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    template <class Range2, class CC = default_channel_converter>
    void write_and_convert (const Range2& range, size_type samples,
			    CC cc = CC ());

    /**
     * Sets to zero all the contents of the buffer.
     */
    void zero ()
    {
	fill_frames (_range, 0); // TODO: USE SAMPLE TRAITS
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
	if (_writepos._count < size ())
	    std::swap (_startpos, _writepos._pos);
	// TODO!!! Fix backwards!!
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
	    r._pos = (r._pos + n) % size ();
	    r._add (n);
	}
	else
	{
	    difference_type new_pos = r._pos + n;
	    while (new_pos < 0)
		new_pos += size ();
	    r._pos = new_pos;
	    r._add (n);
	}
    }

    template <typename Position>
    void increment (Position& r) const
    {
	++ r._pos;
	if (r._pos == _range.size ())
	    r._pos = 0;
	r._add (1);
    }

    template <typename Position>
    void decrement (Position& r) const
    {
	-- r._pos;
	if (r._pos < 0)
	    r._pos = _range.size () - 1;
	r._add (-1);
    }

    /**
     * Advances the write pointer of the buffer a given number of elements.
     * @param n The number of elements to advance.
     */
    void advance (size_type n)
    {
	advance (_writepos, n);
    }

    difference_type count () const
    {
	return _writepos.count ();
    }
    
    /**
     * Returns a new iterator in the same position but with the read count
     * calculated from the read position to the write position.
     * This function may be called to get back from an erratic iterator
     * and @b must be called whenever the write direction is changed using
     * the @c backwards () function.
     */
    safe_position sync (const safe_position& r) const
    {
	if (!_backwards)
	    return safe_iterator (
		r._pos, _writepos._count - (r._pos <= _writepos._pos ?
					    _writepos._pos - r._pos :
					    size () - r._pos + _writepos._pos));
	else
	    return safe_iterator (
		r._pos,	_writepos._count - (r._pos >= _writepos._pos ?
					    r._pos - _writepos._pos :
					    size () - _writepos._pos + r._pos));
    }
    
protected:
    bool          _backwards;  /**< @c true if we are reading and
			          writting the ringbuffer backwards. */
    size_type     _startpos;   /**< The new starting position of the
				  ring buffer. */
    safe_position _writepos;
    Range         _range;
};

template <class Range>
class ring_buffer_range : public ring_buffer_range_base<Range>
{
    typedef ring_buffer_range_base<Range> parent_type;

public:
    typedef typename parent_type::range::value_type         value_type;
    typedef typename parent_type::range::reference          reference;
    typedef typename parent_type::range::difference_type    difference_type;
    
    typedef detail::ring_iterator<typename parent_type::position,
				  ring_buffer_range>
    iterator;
    typedef detail::ring_iterator<typename parent_type::safe_position,
				  ring_buffer_range>
    safe_iterator;

    /**
     * Constructor.
     * @param size The size of the buffer, defaults to zero.
     */
    ring_buffer_range () {}

    /** Copy constructor */
    ring_buffer_range (const ring_buffer_range& range)
	: parent_type ((const parent_type&) range) {}

    template<class Range2>
    explicit ring_buffer_range (Range2& range)
	: parent_type (range) {}
    
    template<class Range2>
    explicit ring_buffer_range (const Range2& range)
	: parent_type (range) {}
    
    /** Copy constructor. */
    template <class Range2>
    ring_buffer_range (const ring_buffer_range<Range2>& range)
	: parent_type ((const ring_buffer_range_base<Range2>&) range) {}
    
    iterator begin () const
    { return iterator (this->begin_pos (), this); }

    iterator end () const
    { return iterator (this->end_pos (), this); }

    safe_iterator safe_begin () const
    { return safe_iterator (this->safe_begin_pos (), this); }

    safe_iterator safe_end () const
    { return safe_iterator (this->safe_end_pos (), this); }

    /**
     * @note This should not be implemented in terms of iterator::operator*!
     */
    reference operator [] (difference_type i) const
    { return this->_range [i]; } // potential performance problem!
    
    iterator at (difference_type i) const
    { return begin () + i; }
};

template <class Range>
struct const_ring_buffer_range :
    public ring_buffer_range<typename Range::const_type> {};

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

#include <psynth/sound/ring_buffer_range.tpp>

#endif /* PSYNTH_SOUND_RING_BUFFER_RANGE_H_ */

