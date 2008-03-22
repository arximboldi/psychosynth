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

#include <algorithm>
#include <iostream>
#include <cmath>

#include "common/ControlBuffer.h"
#include "common/RingControlBuffer.h"

using namespace std;

namespace psynth
{

void RingControlBuffer::allocate()
{
    m_data = new Sample  [ m_size ];
}

RingControlBuffer::RingControlBuffer() :
    m_data(NULL),
    m_fr_count(0)
{
};

RingControlBuffer::RingControlBuffer(int size) :
    RingBuffer(size),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
};

RingControlBuffer::RingControlBuffer(const RingControlBuffer& buf) :
    RingBuffer(buf),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
    memcpy(m_data, buf.m_data, sizeof(Sample) * m_size);
};

RingControlBuffer& RingControlBuffer::operator= (RingControlBuffer& buf)
{
    if (&buf != this) {
	if (buf.m_size != m_size) {
	    liberate();
	    m_size = buf.m_size;
	    allocate();
	}
	
	memcpy(m_data, buf.m_data, sizeof(Sample) * m_size);
    }
    
    return *this;
};

int RingControlBuffer::read(ReadPtr& r, ControlBuffer& buf, int samples) const
{
    int nread = min(availible(r), samples);
	
    if (position(r) + nread > m_size) {
	memcpy(buf.getData(), m_data + position(r),
	       sizeof(Sample) * (m_size - position(r)));
	memcpy(buf.getData() + m_size - position(r), m_data,
	       sizeof(Sample) * (position(r) + nread - m_size));
    } else {
	memcpy(buf.getData(), m_data + position(r), sizeof(Sample) * nread);
    }
	
    advance(r, nread);
	
    return nread;
}

void RingControlBuffer::write(const ControlBuffer& buf, int nwrite)
{
    int offset = 0;
    if (nwrite > m_size) {
	offset = nwrite - m_size;
	nwrite = m_size;
    }
    
    if (m_writepos + nwrite > m_size) {
	memcpy(m_data + m_writepos, buf.getData() + offset,
	       sizeof(Sample) * (m_size - m_writepos));
	memcpy(m_data, buf.getData() + offset + m_size - m_writepos, 
	       sizeof(Sample) * (m_writepos + nwrite - m_size));	
    } else {
	memcpy(m_data + m_writepos, buf.getData() + offset, sizeof(Sample) * nwrite);
    }
	
    advance(nwrite);
}

void RingControlBuffer::writeFastResample(const ControlBuffer& buf, int samples, float factor)
{
    int i;
    int real_samples = ceil(samples * (1.0f / factor));

    if (real_samples > m_size) {
	real_samples -= m_size;
	m_fr_count += (real_samples - real_samples) * factor;
    }

    if (m_fr_count < samples) {
	if (m_writepos + real_samples > m_size) {
	    for (i = m_writepos; i < m_size; m_fr_count += factor, ++i)
		m_data[i] = buf.getData()[(int)m_fr_count];
	
	    for (i = 0; m_fr_count < samples; m_fr_count += factor, ++i)
		m_data[i] = buf.getData()[(int)m_fr_count];
	} else {
	    for (i = m_writepos; m_fr_count < samples; m_fr_count += factor, ++i)
		m_data[i] = buf.getData()[(int)m_fr_count];
	}
	
	m_writepos = i % m_size;
	m_writecount += real_samples;
    }
    
    m_fr_count -= samples;
}

} /* namespace psynth*/
