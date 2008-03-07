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

#ifndef PSYNTH_RINGCONTROLBUFFER
#define PSYNTH_RINGCONTROLBUFFER

#include <libpsynth/common/ControlBuffer.h>

namespace psynth
{

class RingControlBuffer
{
public:
    class ReadPtr {
	friend class RingControlBuffer;
		
	int m_pos;
	int m_count;
    public:
	ReadPtr(int pos = 0, int count = 0) :
	    m_pos(pos),
	    m_count(count) {}
    };

    enum {
	ERR_NONE,
	ERR_UNDERRUN,
	ERR_OVERRUN,
	ERR_CODES
    };
	
private:
    Sample* m_data;
    int m_size;
    int m_writepos;
    int m_writecount;
    float m_fr_count;

    void allocate();
	
    void liberate() {
	delete [] m_data;
    }

public:
    RingControlBuffer();
	
    RingControlBuffer(int size);
	
    RingControlBuffer(const RingControlBuffer& buf);
	
    ~RingControlBuffer() {
	liberate();
    }
	
    RingControlBuffer& operator= (RingControlBuffer& buf);
	    
    int size() const {
	return m_size;
    }

    void resize(int new_size) {
	liberate();
	m_size = new_size;
	allocate();
    }
    
    ReadPtr begin() const {
	return ReadPtr(m_writepos, m_writecount - m_size < 0 ?
		       m_writecount - m_size : 0);
    };
	
    ReadPtr end() const {
	return ReadPtr(m_writepos, m_writecount);
    };
	
    int availible(const ReadPtr& r) const {
	return m_writecount - r.m_count;
    }
	
    int error(const ReadPtr& reader) const {
	if (reader.m_pos + m_size < m_writepos) return ERR_UNDERRUN;
	if (reader.m_count > m_writecount) return ERR_OVERRUN;
	return ERR_NONE;
    }
	
    int read(ReadPtr& r, ControlBuffer& buf) const {
	return read(r, buf, buf.size());
    };
	
    int read(ReadPtr& r, ControlBuffer& buf, int samples) const;
	
    void write(const ControlBuffer& buf) {
	write(buf, buf.size());
    }
	
    void write(const ControlBuffer& buf, int samples);

    void writeFastResample(const ControlBuffer& buf, float factor) {
	writeFastResample(buf, buf.size(), factor);
    }
    
    void writeFastResample(const ControlBuffer& buf, int samples, float factor);

    void zero() {
	memset(m_data, 0, m_size * sizeof(Sample));
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGCONTROLBUFFER */
