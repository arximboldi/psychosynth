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

#include "common/ring_audio_buffer.hpp"
#include "common/misc.hpp"
#include "common/logger.hpp"
#include "common/scaler.hpp"

using namespace std;

namespace psynth
{

void ring_audio_buffer::allocate()
{
    int i;
    if (m_info.num_channels && m_size) {
	m_data  = new sample* [ m_info.num_channels ];
	*m_data = new sample  [ m_size * m_info.num_channels ];
	for (i = 1; i < m_info.num_channels; i++)
	    m_data[i] = m_data[i-1] + m_size;
    }
}

ring_audio_buffer::ring_audio_buffer() :
    m_info(0,0,0),
    m_data(NULL),
    m_fr_count(0)
{
};

ring_audio_buffer::ring_audio_buffer(int size) :
    ring_buffer(size),
    m_info(0,0,0),
    m_data(NULL),
    m_fr_count(0)
{
};

ring_audio_buffer::ring_audio_buffer(const audio_info& info):
    ring_buffer(info.block_size),
    m_info(info),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
};

ring_audio_buffer::ring_audio_buffer(const audio_info& info, int size):
    ring_buffer(size),
    m_info(info),
    m_data(NULL),
    m_fr_count(0)
{
    allocate();
};

ring_audio_buffer::ring_audio_buffer(const ring_audio_buffer& buf) :
    ring_buffer(buf),
    m_info(buf.m_info),
    m_data(NULL),
    m_fr_count(buf.m_fr_count)
{
    allocate();
    memcpy(*m_data, *buf.m_data, sizeof(sample) * m_info.num_channels * m_size);
};

ring_audio_buffer& ring_audio_buffer::operator= (ring_audio_buffer& buf)
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
	memcpy(*m_data, *buf.m_data, sizeof(sample) * m_info.num_channels * m_size);
    }
    return *this;
};

int ring_audio_buffer::read(read_ptr& r, audio_buffer& buf, int samples) const
{
    int nread = min(availible(r), samples);

    if (is_backwards ())
	advance(r, -nread);
    
    for (int i = 0; i < m_info.num_channels; i++) {
	if (position(r) + nread > m_size) {
	    memcpy(buf[i], m_data[i] + position(r), 
		   sizeof(sample) * (m_size - position(r)));
	    memcpy(buf[i] + m_size - position(r), m_data[i],
		   sizeof(sample) * (position(r) + nread - m_size));
			
	} else {
	    memcpy(buf[i], m_data[i] + position(r), sizeof(sample) * nread);
	}
    }

    if (!is_backwards ())
	advance(r, nread);
    
    return nread;
}

void ring_audio_buffer::write(const audio_buffer& buf, int nwrite)
{
    int offset = 0;

    if (nwrite > m_size) {
	logger::instance() ("core", log::WARNING, "Ring buffer overflow.");
	return;
    }

    if (is_backwards ())
	advance(-nwrite);
    
    for (int i = 0; i < m_info.num_channels; i++) {
	if (m_writepos + nwrite > m_size) {
	    memcpy(m_data[i] + m_writepos, buf[i] + offset,
		   sizeof(sample) * (m_size - m_writepos));
	    memcpy(m_data[i], buf[i] + offset + m_size - m_writepos, 
		   sizeof(sample) * (m_writepos + nwrite - m_size));
			
	} else {
	    memcpy(m_data[i] + m_writepos, buf[i] + offset, sizeof(sample) * nwrite);
	}
    }
    
    if(!is_backwards ())
	advance(nwrite);
}

void ring_audio_buffer::deinterleave(const sample* buf, int to_write)
{
    if (to_write > m_size) {
	logger::instance() ("core", log::WARNING, "Ring buffer overflow.");
	return;
    }

    const sample* inbuf;
    sample* outbuf;
    int j;
    int nwrite;

    if (is_backwards ())
	advance(-to_write);
    
    for (int i = 0; i < m_info.num_channels; i++) {
	nwrite = to_write;
	if (m_writepos + nwrite > m_size) {
	    outbuf = m_data[i] + m_writepos;
	    inbuf = buf + i;
	    for (j = m_writepos; j < m_size; ++j, --nwrite) {
		*outbuf++ = *inbuf;
		inbuf += m_info.num_channels;
	    }
	    outbuf = m_data[i];
	    while(nwrite-- > 0) {
		*outbuf++ = *inbuf;
		inbuf += m_info.num_channels;
	    }
	} else {
	    outbuf = m_data[i] + m_writepos;
	    inbuf = buf + i;
	    while (nwrite-- > 0) {
		*outbuf++ = *inbuf;
		inbuf += m_info.num_channels;
	    }
	}
    }

    if (!is_backwards ())
	advance(to_write);
}

void ring_audio_buffer::write_scaler(const audio_buffer& inbuf, int samples,
				     scaler& scaler)
{
    sample buf[samples * inbuf.get_info().num_channels];
    int nframes;
    
    inbuf.interleave(buf, samples);

    scaler.update(buf, samples);

    while (scaler.availible() > 0) {
	nframes = scaler.receive(buf, samples);
	deinterleave(buf, nframes);
    }
}

void ring_audio_buffer::write_fast_resample(const audio_buffer& buf, int samples,
					    float factor)
{
    samples--;
    float count = m_fr_count;
    int i, j;
    int real_samples = ceil((float)samples / factor);

    if (real_samples > m_size) {
	real_samples = m_size;
	samples = (float) real_samples * factor;
        //logger::instance() ("core", log::WARNING, "Ring buffer overflow.");
	//return;
    }
    
    if (m_fr_count < samples) {
	for (j = 0; j < m_info.num_channels; ++j) {
	    if (m_writepos + real_samples > m_size) {
		for (i = m_writepos, count = m_fr_count; i < m_size; count += factor, ++i)
		    m_data[j][i] = linear_interp(buf[j][(int)count],
						 buf[j][(int)count + 1],
						 phase(count));
	
		for (i = 0; count < samples; count += factor, ++i)
		    m_data[j][i] = linear_interp(buf[j][(int)count],
						 buf[j][(int)count + 1],
						 phase(count));
	    } else {
		for (i = m_writepos, count = m_fr_count; count < samples; count += factor, ++i)
		    m_data[j][i] = linear_interp(buf[j][(int)count],
						 buf[j][(int)count + 1],
						 phase(count));
	    }
	}

	m_writepos = i % m_size;
	m_writecount += real_samples;
    }

    m_fr_count = count - samples;    
}

} /* namespace psynth */

