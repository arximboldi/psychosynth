/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef PSYNTH_SAMPLE_BUFFER_H
#define PSYNTH_SAMPLE_BUFFER_H

#include <cstring>

#include <libpsynth/common/audio_info.h>

namespace psynth
{

/**
 * A buffer for control data. It holds only one channel of scalar samples
 * and contains no information about sampling rate or other audio signal related
 * information, in contrast with @c AudioBuffer.
 */
class sample_buffer
{
    sample* m_data;
    size_t m_size;

    void allocate() {
	m_data = new sample[m_size];
        /* FIXME: what is the effect of new sample[0] ?*/
    }
    
    void liberate() {
	delete [] m_data;
	m_data = NULL;
    }
    
public:
    /** Constructor. */
    sample_buffer () :
	m_data(NULL),
	m_size(0)
	{}

    /** Destructor. */
    sample_buffer (size_t size) :
	m_size(size) {
	allocate();
    }

    /** Copy constructor. */
    sample_buffer (const sample_buffer& buf) :
	m_size(buf.m_size) {
	allocate();
	memcpy(m_data, buf.m_data, sizeof(sample) * m_size);
    }

    /** Destructor. */
    ~sample_buffer() {
	liberate();
    }

    /** Assignment operator. */
    sample_buffer& operator= (const sample_buffer& buf) {
	if (&buf != this) {
	    liberate();
	    m_size = buf.m_size;
	    allocate();
	    memcpy(m_data, buf.m_data, sizeof(sample) * m_size);
	}
	return *this;
    }

    /**
     * Returns a reference to a sample contained in the buffer.
     * @param i Index to the sample we want.
     */
    sample& operator[] (size_t i) {
	return m_data[i];
    }

    /**
     * Returns a constant reference to a sample contained in the buffer.
     * @param i Index to the sample we want.
     */
    const sample& operator[] (size_t i) const {
	return m_data[i];
    }

    /**
     * Returns a pointer to the raw data of the buffer.
     */
    sample* get_data() {
	return m_data;
    }

    /**
     * Returns a constant pointer to the raw data of the buffer.
     */
    const sample* get_data() const {
	return m_data;
    }

    /**
     * Free all the data in the buffer setting its size to 0.
     */
    void clear () {
	liberate();
	m_size = 0;
    }

    /**
     * Returns the current size of the buffer.
     */
    size_t size () const {
	return m_size;
    }

    /**
     * Gives the buffer a new size.
     * @param newsize The new size of the buffer.
     */
    void resize (size_t newsize) {
	liberate ();
	m_size = newsize;
	allocate ();
    }

    /**
     * Finds the next hill. We define a hill as a secuence of consecutive non
     * zero values plus any trailing zeros until the next non zero value.
     * This is quite usefull to analize trigger data.
     * @param start Where to start looking for the hill.
     * @return The position of the first non zero value after the hill.
     */
    size_t find_hill (size_t start = 0) const;

    /**
     * Returns wether a point may be the end of a hill.
     * @see findHill
     */
    bool is_hill_end (size_t end) const {
	if (end == 0)
	    return false;
	else if (m_data[end-1] != 0)
	    return false;
	
	return true;
    }
    
    /**
     * Fills the whole buffer with value zero.
     */
    void zero () {
	memset(m_data, 0, sizeof(sample) * m_size);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_CONTROLBUFER_H */
