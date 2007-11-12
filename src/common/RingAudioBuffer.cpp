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

#include <iostream>
#include "common/RingAudioBuffer.h"

using namespace std;

void RingAudioBuffer::allocate()
{
    int i;
    m_data  = new Sample* [ m_info.num_channels ];
    *m_data = new Sample  [ m_size * m_info.num_channels ];
    for (i = 1; i < m_info.num_channels; i++)
	m_data[i] = m_data[i-1] + m_size;
}

RingAudioBuffer::RingAudioBuffer() :
    m_info(0,0,0),
    m_data(NULL),
    m_size(0),
    m_writepos(0),
    m_writecount(0)
{
};

RingAudioBuffer::RingAudioBuffer(const AudioInfo& info):
    m_info(info),
    m_data(NULL),
    m_size(info.block_size),
    m_writepos(0),
    m_writecount(0)
{
    allocate();
};

RingAudioBuffer::RingAudioBuffer(const AudioInfo& info, int size):
    m_info(info),
    m_data(NULL),
    m_size(size),
    m_writepos(0),
    m_writecount(0)
{
    allocate();
};

RingAudioBuffer::RingAudioBuffer(const RingAudioBuffer& buf) :
    m_info(buf.m_info),
    m_data(NULL),
    m_size(buf.m_size),
    m_writepos(buf.m_writepos),
    m_writecount(buf.m_writecount)
{
    allocate();
    memcpy(*m_data, *buf.m_data, sizeof(Sample) * m_info.num_channels * m_size);
};

RingAudioBuffer& RingAudioBuffer::operator= (RingAudioBuffer& buf)
{
    if (&buf != this) {
	if (buf.m_size != m_size || 
	    buf.m_info.num_channels != buf.m_info.num_channels) {
	    liberate();
	    m_info = buf.m_info;
	    allocate();
	} else
	    m_info.sample_rate = buf.m_info.sample_rate;
	memcpy(*m_data, *buf.m_data, sizeof(Sample) * m_info.num_channels * m_size);
    }
    return *this;
};

int RingAudioBuffer::read(ReadPtr& r, AudioBuffer& buf, int samples) const
{
    int nread = min(availible(r), samples);
	
    for (int i = 0; i < m_info.num_channels; i++) {
	if (r.m_pos + nread > m_size) {
	    memcpy(buf[i], m_data[i] + r.m_pos, 
		   sizeof(Sample) * (m_size - r.m_pos));
	    memcpy(buf[i] + m_size - r.m_pos, m_data[i],
		   sizeof(Sample) * (r.m_pos + nread - m_size));
			
	} else {
	    memcpy(buf[i], m_data[i] + r.m_pos, sizeof(Sample) * nread);
	}
    }
	
    r.m_pos = (r.m_pos + nread) % m_size;
    r.m_count += nread;
	
    return nread;
}

void RingAudioBuffer::write(const AudioBuffer& buf, int nwrite)
{
    for (int i = 0; i < m_info.num_channels; i++) {
	if (m_writepos + nwrite > m_size) {
	    memcpy(m_data[i] + m_writepos, buf[i],
		   sizeof(Sample) * (m_size - m_writepos));
	    memcpy(m_data[i], m_data[i] + m_size - m_writepos, 
		   sizeof(Sample) * (m_writepos + nwrite - m_size));
			
	} else {
	    memcpy(m_data[i] + m_writepos, buf[i], sizeof(Sample) * nwrite);
	}
    }
	
    m_writepos = (m_writepos + nwrite) % m_size;
    m_writecount += nwrite;
}

