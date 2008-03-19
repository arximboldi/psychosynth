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

namespace psynth
{

class Scaler;

class RingAudioBuffer
{
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

    float m_fr_count;
    float m_prev_val;
    
    void allocate();
	
    void liberate() {
	if (m_data) {
	    delete [] *m_data;
	    delete [] m_data;
	}
    }

public:
    RingAudioBuffer();

    RingAudioBuffer(int size);
    
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

    void setAudioInfo(const AudioInfo& info) {
	if (info.num_channels != m_info.num_channels) {
	    liberate();
	    m_info = info;
	    allocate();
	} else
	    m_info = info;
    }

    void setAudioInfo(const AudioInfo& info, int new_size) {
	if (info.num_channels != m_info.num_channels
	    || m_size != new_size) {
	    liberate();
	    m_size = new_size;
	    m_info = info;
	    allocate();
	} else
	    m_info = info;
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
	return ReadPtr(m_writepos,
		       m_writecount - m_size < 0 ?
		       0 :
		       m_writecount - m_size);
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
	return read(r, buf, buf.size());
    };
	
    int read(ReadPtr& r, AudioBuffer& buf, int samples) const;

    void deinterleave(const Sample* buf, int samples);
    
    void write(const AudioBuffer& buf) {
	write(buf, buf.size());
    }

    void writeScaler(const AudioBuffer& buf, Scaler& scaler) {
	writeScaler(buf, buf.size(), scaler);
    }
    
    void writeScaler(const AudioBuffer& buf, int samples, Scaler& scaler);
    
    void writeFastResample(const AudioBuffer& buf, float factor) {
	writeFastResample(buf, buf.size(), factor);
    }
    
    void writeFastResample(const AudioBuffer& buf, int samples, float factor);
    
    void write(const AudioBuffer& buf, int samples);

    void zero() {
	if (m_data)
	    memset(*m_data, 0, sizeof(Sample) * m_size * m_info.num_channels);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGAUDIOBUFFER_H */
