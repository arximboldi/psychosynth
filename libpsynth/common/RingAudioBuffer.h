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

#ifndef PSYNTH_RINGAUDIOBUFFER_H
#define PSYNTH_RINGAUDIOBUFFER_H

#include <libpsynth/common/AudioInfo.h>
#include <libpsynth/common/AudioBuffer.h>

/*
 * TODO MULTITHREADING!
 */ 

namespace psynth
{

class RingAudioBuffer {
public:
    class ReadPtr {
	friend class RingAudioBuffer;
		
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
    AudioInfo m_info;
    Sample** m_data;
    int m_size;
    int m_writepos;
    int m_writecount;
	
    void allocate();
	
    void liberate() {
	delete [] *m_data;
	delete [] m_data;
    }

public:
    RingAudioBuffer();
	
    RingAudioBuffer(const AudioInfo& info);
	
    RingAudioBuffer(const AudioInfo& info, int size);
	
    RingAudioBuffer(const RingAudioBuffer& buf);
	
    ~RingAudioBuffer() {
	liberate();
    }
	
    RingAudioBuffer& operator= (RingAudioBuffer& buf);
	
    const AudioInfo& getAudioInfo() const {
	return m_info;
    }
    
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
	
    int read(ReadPtr& r, AudioBuffer& buf) const {
	return read(r, buf, buf.getInfo().block_size);
    };
	
    int read(ReadPtr& r, AudioBuffer& buf, int samples) const;
	
    void write(const AudioBuffer& buf) {
	write(buf, buf.getInfo().block_size);
    }
	
    void write(const AudioBuffer& buf, int samples);
};

} /* namespace psynth */

#endif /* PSYNTH_RINGAUDIOBUFFER_H */
