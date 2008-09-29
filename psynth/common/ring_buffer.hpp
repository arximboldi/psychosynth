/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef PSYNTH_RING_BUFFER_H
#define PSYNTH_RING_BUFFER_H

#include <algorithm>

namespace psynth
{

/**
 * Base ring buffer class. Defines some common elements and behavour among
 * ring buffers.
 */
class ring_buffer
{
public:
    /**
     * A pointer a reader can use to read data from the buffer.
     */
    class read_ptr
    {
	friend class ring_buffer;
		
	int m_pos;
	int m_count;
    public:
	/**
	 * Constructor.
	 * @param pos Starting position in the buffer.
	 * @param count Number of samples read.
	 */
	read_ptr(int pos = 0, int count = 0) :
	    m_pos(pos),
	    m_count(count)
	    {}

	/**
	 * Returns the total number of elements read.
	 */
	int get_count() {
	    return m_count;
	};
    };

    /**
     * Error codes for a read pointer.
     */
    enum error {
	/** No error. */
	ERR_NONE,
	/**
	 * An underrun has ocurred. This means that there has been too much
	 * data written overwritting data thas has not been previously read.
	 */
	ERR_UNDERRUN,
	/**
	 * An overrun has ocurred. This means that the reader has read more data
	 * that the one actually in the buffer.
	 */
	ERR_OVERRUN,
	/** Number of error codes.*/
	ERR_CODES
    };

    bool m_backwards; /**< @c true if we are reading and writting the
			 ringbuffer backwards. */
    int m_startpos;  /**< The new starting position of the ring buffer. */
	
protected:
    int m_size;       /**< The size of the buffer. */
    int m_writepos;   /**< The current writting position of the buffer. */
    int m_writecount; /**< Total number of elements written to the buffer. */ 
    		 
    /**
     * Accessor function to allow childs get and set a ReadPtr position.
     * @param r The read_ptr to consult.
     */
    int& position (read_ptr& r) const {
	return r.m_pos;
    };

    /**
     * Accessor function to allow childs get and set a ReadPtr count.
     * @param r The read_ptr to consult.
     */
    int& count (read_ptr& r) const {
	return r.m_count;
    }

    /**
     * Advances a read pointer a given number of elements.
     * @param r The read_ptr to advance.
     * @param n The number of elements to advance.
     */
    void advance (read_ptr& r, int n) const {
	if (n >= 0) {
	    r.m_pos = (r.m_pos + n) % m_size;
	    r.m_count += n;
	} else {
	    r.m_pos += n;
	    while(r.m_pos < 0)
		r.m_pos += m_size;
	    r.m_count -= n;
	}
    }

    /**
     * Advances the write pointer of the buffer a given number of elements.
     * @param n The number of elements to advance.
     */
    void advance (int n) {
	if (n >= 0) {
	    m_writepos = (m_writepos + n) % m_size;
	    m_writecount += n;
	} else {
	    m_writepos += n;
	    while(m_writepos < 0)
		m_writepos += m_size;
	    m_writecount -= n;
	}
    }

    /**
     * Constructor.
     * @param size The size of the buffer, defaults to zero.
     */
    ring_buffer (int size = 0) :
	m_size(size),
	m_backwards(false),
	m_writepos(0),
	m_writecount(0),
	m_startpos(0)
	{}

    /** Copy constructor */
    ring_buffer (const ring_buffer& buf) :
	m_size(buf.m_size),
	m_writepos(buf.m_writepos),
	m_writecount(buf.m_writecount),
	m_backwards(buf.m_backwards),
	m_startpos(0)
	{}
    
public:
    
    /**
     * Returns the size of the buffer.
     */
    int size () const {
	return m_size;
    }

    /**
     * Returns a read pointer to the beginning of the availible data.
     */
    read_ptr begin () const {
	if (m_writecount < m_size)
	    return read_ptr (m_startpos, 0);
	else
	    return read_ptr (m_writepos,
			     m_writecount - m_size);
    };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    read_ptr end () const {
	return read_ptr (m_writepos, m_writecount);
    };

    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    int availible (const read_ptr& r) const {
	return m_writecount - r.m_count;
    }

    /**
     * Checks a read_ptr for error states.
     */
    int error (const read_ptr& reader) const {
	if (reader.m_count < m_writecount - m_size)
	return ERR_UNDERRUN;
	if (reader.m_count > m_writecount)
	return ERR_OVERRUN;
	return ERR_NONE;
    }

    /**
     * Returns wether this buffer is being written backwards.
     */
    bool is_backwards () const {
	return m_backwards;
    }
    
    /**
     * Changes the reading direction of the current pointer write pointer.
     * If you are using this buffer as an intermediate buffer from another
     * medium and your where reading that medium forwards, this allows you to
     * not to lose the already read data remaining in the buffer if you
     * change the reading direction.
     * @see sync()
     */
    void backwards () {
	m_backwards = !m_backwards;
	if (m_writecount < m_size)
	    std::swap(m_startpos, m_writepos);
    }

    /**
     * Returns a new read_ptr in the same position but with the read count
     * calculated from the read position to the write position.
     * This function may be called to get back from an erratic read_ptr
     * and @b must be called whenever the write direction is changed using
     * the @c backwards() function.
     */
    read_ptr sync (const read_ptr& r) {
	if (!m_backwards)
	    return read_ptr (r.m_pos,
			     m_writecount -
			     (r.m_pos <= m_writepos ?
			      m_writepos - r.m_pos :
			      m_size - r.m_pos + m_writepos));
	else
	    return read_ptr (r.m_pos,
			     m_writecount -
			     (r.m_pos >= m_writepos ?
			      r.m_pos - m_writepos :
			      m_size - m_writepos + r.m_pos));
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGBUFFER */

