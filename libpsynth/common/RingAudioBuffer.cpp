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
#include <cmath>
#include "common/RingAudioBuffer.h"

using namespace std;

namespace psynth
{

void RingAudioBuffer::allocate()
{
    int i;
    if (m_info.num_channels && m_size) {
	m_data  = new Sample* [ m_info.num_channels ];
	*m_data = new Sample  [ m_size * m_info.num_channels ];
	for (i = 1; i < m_info.num_channels; i++)
	    m_data[i] = m_data[i-1] + m_size;
    }
}

RingAudioBuffer::RingAudioBuffer() :
    m_info(0,0,0),
    m_data(NULL),
    m_size(0),
    m_writepos(0),
    m_writecount(0),
    m_fr_count(0)
{
};

RingAudioBuffer::RingAudioBuffer(int size) :
    m_info(0,0,0),
    m_data(NULL),
    m_size(size),
    m_writepos(0),
    m_writecount(0),
    m_fr_count(0)
{
};

RingAudioBuffer::RingAudioBuffer(const AudioInfo& info):
    m_info(info),
    m_data(NULL),
    m_size(info.block_size),
    m_writepos(0),
    m_writecount(0),
    m_fr_count(0)
{
    allocate();
};

RingAudioBuffer::RingAudioBuffer(const AudioInfo& info, int size):
    m_info(info),
    m_data(NULL),
    m_size(size),
    m_writepos(0),
    m_writecount(0),
    m_fr_count(0)
{
    allocate();
};

RingAudioBuffer::RingAudioBuffer(const RingAudioBuffer& buf) :
    m_info(buf.m_info),
    m_data(NULL),
    m_size(buf.m_size),
    m_writepos(buf.m_writepos),
    m_writecount(buf.m_writecount),
    m_fr_count(buf.m_fr_count)
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
	    m_size = buf.m_size;
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
    int offset = 0;
    if (nwrite > m_size) {
	offset = nwrite - m_size;
	nwrite = m_size;
    }
    
    for (int i = 0; i < m_info.num_channels; i++) {
	if (m_writepos + nwrite > m_size) {
	    memcpy(m_data[i] + m_writepos, buf[i] + offset,
		   sizeof(Sample) * (m_size - m_writepos));
	    memcpy(m_data[i], buf[i] + offset + m_size - m_writepos, 
		   sizeof(Sample) * (m_writepos + nwrite - m_size));
			
	} else {
	    memcpy(m_data[i] + m_writepos, buf[i] + offset, sizeof(Sample) * nwrite);
	}
    }
	
    m_writepos = (m_writepos + nwrite) % m_size;
    m_writecount += nwrite;
}

void RingAudioBuffer::writeFastResample(const AudioBuffer& buf, int samples,
					float factor)
{
    float count = m_fr_count;
    int i, j;
    int real_samples = ceil(samples * (1.0f / factor));

    if (real_samples > m_size) {
	real_samples -= m_size;
	m_fr_count += (real_samples - real_samples) * factor;
    }
    
    if (m_fr_count < samples) {
	for (j = 0; j < m_info.num_channels; ++j) {
	    if (m_writepos + real_samples > m_size) {
		for (i = m_writepos, count = m_fr_count; i < m_size; count += factor, ++i)
		    m_data[j][i] = buf[j][(int)count];
	
		for (i = 0; count < samples; count += factor, ++i)
		    m_data[j][i] = buf[j][(int)count];
	    } else {
		for (i = m_writepos, count = m_fr_count; count < samples; count += factor, ++i)
		    m_data[j][i] = buf[j][(int)count];
	    }
	}

	m_writepos = i % m_size;
	m_writecount += real_samples;
    }

    m_fr_count = count - samples;    
}

} /* namespace psynth */

