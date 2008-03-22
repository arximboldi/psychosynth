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

#ifndef PSYNTH_RINGBUFFER
#define PSYNTH_RINGBUFFER

namespace psynth
{

/**
 * Base ring buffer class. Defines some common elements and behavour among
 * ring buffers.
 */
class RingBuffer
{
public:
    /**
     * A pointer a reader can use to read data from the buffer.
     */
    class ReadPtr
    {
	friend class RingBuffer;
		
	int m_pos;
	int m_count;
    public:
	/**
	 * Constructor.
	 * @param pos Starting position in the buffer.
	 * @param count Number of samples read.
	 */
	ReadPtr(int pos = 0, int count = 0) :
	    m_pos(pos),
	    m_count(count)
	    {}

	/**
	 * Returns the total number of elements read.
	 */
	int getCount() {
	    return m_count;
	};
    };

    /**
     * Error codes for a read pointer.
     */
    enum Error {
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
    
protected:
    int m_size;       /**< The size of the buffer. */
    int m_writepos;   /**< The current writting position of the buffer. */
    int m_writecount; /**< Total number of elements written to the buffer. */ 

    /**
     * Accessor function to allow childs get and set a ReadPtr position.
     * @param r The ReadPtr to consult.
     */
    int& position(ReadPtr& r) const {
	return r.m_pos;
    };

    /**
     * Accessor function to allow childs get and set a ReadPtr count.
     * @param r The ReadPtr to consult.
     */
    int& count(ReadPtr& r) const {
	return r.m_count;
    }

    /**
     * Advances a read pointer a given number of elements.
     * @param r The ReadPtr to advance.
     * @param n The number of elements to advance.
     */
    void advance(ReadPtr& r, int n) const {
	r.m_pos = (r.m_pos + n) % m_size;
	r.m_count += n;
    }

    /**
     * Advances the write pointer of the buffer a given number of elements.
     * @param n The number of elements to advance.
     */
    void advance(int n) {
	m_writepos = (m_writepos + n) % m_size;
	m_writecount += n;
    }

    /**
     * Constructor.
     * @param size The size of the buffer, defaults to zero.
     */
    RingBuffer(int size = 0) :
	m_size(size),
	m_writepos(0),
	m_writecount(0)
	{}

    /** Copy constructor */
    RingBuffer(const RingBuffer& buf) :
	m_size(buf.m_size),
	m_writepos(buf.m_writepos),
	m_writecount(buf.m_writecount)
	{}
    
public:
    
    /**
     * Returns the size of the buffer.
     */
    int size() const {
	return m_size;
    }

        /**
     * Returns a read pointer to the beginning of the availible data.
     */
    ReadPtr begin() const {
	return ReadPtr(m_writepos,
		       m_writecount - m_size < 0 ?
		       0 :
		       m_writecount - m_size);
    };

    /**
     * Returns a read pointer to the end of the availible data.
     */
    ReadPtr end() const {
	return ReadPtr(m_writepos, m_writecount);
    };

    /**
     * Returns the number of availible data from a read pointer.
     * @param r The read pointer to test for availible data.
     */
    int availible(const ReadPtr& r) const {
	return m_writecount - r.m_count;
    }

    /**
     * Checks a ReadPtr for error states.
     */
    int error(const ReadPtr& reader) const {
	if (reader.m_count < m_writecount - m_size)
	return ERR_UNDERRUN;
	if (reader.m_count > m_writecount)
	return ERR_OVERRUN;
	return ERR_NONE;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGBUFFER */

