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

#include "synth/sample_buffer.hpp"
#include "synth/ring_sample_buffer.hpp"

using namespace std;

namespace psynth
{

void ring_sample_buffer::allocate ()
{
    m_data = new sample  [ m_size ];
}

ring_sample_buffer::ring_sample_buffer () :
    m_data(NULL),
    m_fr_count(0)
{
};

ring_sample_buffer::ring_sample_buffer (int size) :
    ring_buffer(size),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
};

ring_sample_buffer::ring_sample_buffer(const ring_sample_buffer& buf) :
    ring_buffer(buf),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
    memcpy(m_data, buf.m_data, sizeof(sample) * m_size);
};

ring_sample_buffer& ring_sample_buffer::operator= (ring_sample_buffer& buf)
{
    if (&buf != this) {
	if (buf.m_size != m_size) {
	    liberate();
	    m_size = buf.m_size;
	    allocate();
	}
	
	memcpy(m_data, buf.m_data, sizeof(sample) * m_size);
    }
    
    return *this;
};

int ring_sample_buffer::read(read_ptr& r, sample_buffer& buf, int samples) const
{
    int nread = min(availible(r), samples);
	
    if (position(r) + nread > m_size) {
	memcpy(buf.get_data (), m_data + position(r),
	       sizeof(sample) * (m_size - position(r)));
	memcpy(buf.get_data () + m_size - position(r), m_data,
	       sizeof(sample) * (position(r) + nread - m_size));
    } else {
	memcpy(buf.get_data (), m_data + position(r), sizeof(sample) * nread);
    }
	
    advance(r, nread);
	
    return nread;
}

void ring_sample_buffer::write(const sample_buffer& buf, int nwrite)
{
    int offset = 0;
    if (nwrite > m_size) {
	offset = nwrite - m_size;
	nwrite = m_size;
    }
    
    if (m_writepos + nwrite > m_size) {
	memcpy(m_data + m_writepos, buf.get_data () + offset,
	       sizeof(sample) * (m_size - m_writepos));
	memcpy(m_data, buf.get_data () + offset + m_size - m_writepos, 
	       sizeof(sample) * (m_writepos + nwrite - m_size));	
    } else {
	memcpy(m_data + m_writepos, buf.get_data () + offset, sizeof(sample) * nwrite);
    }
	
    advance(nwrite);
}

void ring_sample_buffer::write_fast_resample(const sample_buffer& buf, int samples, float factor)
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
		m_data[i] = buf.get_data ()[(int)m_fr_count];
	
	    for (i = 0; m_fr_count < samples; m_fr_count += factor, ++i)
		m_data[i] = buf.get_data ()[(int)m_fr_count];
	} else {
	    for (i = m_writepos; m_fr_count < samples; m_fr_count += factor, ++i)
		m_data[i] = buf.get_data ()[(int)m_fr_count];
	}
	
	m_writepos = i % m_size;
	m_writecount += real_samples;
    }
    
    m_fr_count -= samples;
}

} /* namespace psynth*/
