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

#ifndef PSYNTH_CONTROLBUFFER_H
#define PSYNTH_CONTROLBUFFER_H

#include <cstring>
#include <libpsynth/common/AudioInfo.h>

namespace psynth
{

class ControlBuffer
{
    Sample* m_data;
    size_t m_size;

    void allocate() {
	m_data = new Sample[m_size];
        /* FIXME: what is the effect of new Sample[0] ?*/
    }
    
    void liberate() {
	delete [] m_data;
	m_data = NULL;
    }
    
public:
    ControlBuffer() :
	m_data(NULL),
	m_size(0)
	{}

    ControlBuffer(size_t size) :
	m_size(size) {
	allocate();
    }

    ControlBuffer(const ControlBuffer& buf) :
	m_size(buf.m_size) {
	allocate();
	memcpy(m_data, buf.m_data, sizeof(Sample) * m_size);
    }

    ~ControlBuffer() {
	liberate();
    }
    
    ControlBuffer& operator=(const ControlBuffer& buf) {
	if (&buf != this) {
	    liberate();
	    m_size = buf.m_size;
	    allocate();
	    memcpy(m_data, buf.m_data, sizeof(Sample) * m_size);
	}
	return *this;
    }
    
    Sample& operator[] (size_t i) {
	return m_data[i];
    }

    const Sample& operator[] (size_t i) const {
	return m_data[i];
    }

    Sample* getData() {
	return m_data;
    }

    const Sample* getData() const {
	return m_data;
    }
    
    void clear() {
	liberate();
	m_size = 0;
    }

    size_t size() {
	return m_size;
    }

    void resize(size_t newsize) {
	liberate();
	m_size = newsize;
	allocate();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_CONTROLBUFER_H */
