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

#include <algorithm>
#include "common/AudioBuffer.h"

#define INT_MAX_VALUE_F    2147483647.0
#define SHORT_MAX_VALUE_F  32766.0
#define CHAR_MAX_VALUE_F    128.0

using namespace std;

namespace psynth
{

void AudioBuffer::allocate()
{
    int i;
    m_data  = new Sample* [ m_info.num_channels ];
    *m_data = new Sample  [ m_info.block_size * m_info.num_channels ];
    for (i = 1; i < m_info.num_channels; i++)
	m_data[i] = m_data[i-1] + m_info.block_size;
    memset(*m_data, 0, sizeof(Sample) * m_info.num_channels * m_info.block_size);
}

void AudioBuffer::liberate()
{
    delete [] *m_data;
    delete [] m_data;
}

AudioBuffer& AudioBuffer::operator= (const AudioBuffer& buf)
{
    if (&buf != this) {
	if (buf.m_info.block_size != m_info.block_size || 
	    buf.m_info.num_channels != buf.m_info.num_channels) {
	    liberate();
	    m_info = buf.m_info;
	    allocate();
	} else
	    m_info.sample_rate = buf.m_info.sample_rate;
	memcpy(*m_data, *buf.m_data, sizeof(Sample) * m_info.num_channels * m_info.block_size);
    }
    return *this;
}

void AudioBuffer::interleave(Sample* dest, size_t n_frames) const
{
    size_t i, j;
    Sample* out_buf;
    const Sample* in_buf;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	in_buf = m_data[i];
	out_buf = dest + i;

	for (j = 0; j < n_frames; ++j) {
	    *out_buf = *in_buf++;
	    out_buf += m_info.num_channels;
	}
    }
}

void AudioBuffer::interleaveC8(char* dest, size_t n_frames) const
{
    size_t i, j;
    char* out_buf;
    const Sample* in_buf;
    Sample r;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	in_buf = m_data[i];
	out_buf = dest + i;

	for (j = 0; j < n_frames; ++j) {
	    r = *in_buf++;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf = (short int)(r * CHAR_MAX_VALUE_F);
	    out_buf += m_info.num_channels;
	}
    }
}

void AudioBuffer::interleaveS16(short int* dest, size_t n_frames) const
{
    size_t i, j;
    short int* out_buf;
    const Sample* in_buf;
    Sample r;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	in_buf = m_data[i];
	out_buf = dest + i;

	for (j = 0; j < n_frames; ++j) {
	    r = *in_buf++;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf = (short int)(r * SHORT_MAX_VALUE_F);
	    out_buf += m_info.num_channels;
	}
    }
}

void AudioBuffer::interleaveI32(int* dest, size_t n_frames) const
{
    size_t i, j;
    int* out_buf;
    const Sample* in_buf;
    Sample r;

    for (i = 0; i < m_info.num_channels; ++i) {
	in_buf = m_data[i];
	out_buf = dest + i;

	for (j = 0; j < n_frames; ++j) {
	    r = *in_buf++;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf = (int)(r * INT_MAX_VALUE_F);
	    out_buf += m_info.num_channels;
	}
    }
}

void AudioBuffer::deinterleave(const Sample* src, size_t n_frames, int num_chan)
{
    size_t i, j;
    Sample* out_buf;
    const Sample* in_buf;

    num_chan = min(m_info.num_channels, num_chan);
    
    for (i = 0; i < num_chan; ++i) {
	out_buf = m_data[i];
	in_buf = src + i;

	for (j = 0; j < n_frames; ++j) {
	    *out_buf++ = *in_buf;
	    in_buf += num_chan;
	}
    }

    for (; i < m_info.num_channels; ++i) {
	out_buf = m_data[i];
	in_buf = m_data[i - num_chan];

	for (j = 0; j < n_frames; ++j) {
	    *out_buf++ = *in_buf;
	    in_buf += num_chan;
	}
    }
}

void AudioBuffer::deinterleaveC8(const char* src, size_t n_frames)
{
    size_t i, j;
    Sample* out_buf;
    const char* in_buf;
    Sample r;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	out_buf = m_data[i];
	in_buf = src + i;

	for (j = 0; j < n_frames; ++j) {
	    r = (float) *in_buf / CHAR_MAX_VALUE_F;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf++ = r;
	    in_buf += m_info.num_channels;
	}
    }    
}

void AudioBuffer::deinterleaveS16(const short int* src, size_t n_frames)
{
    size_t i, j;
    Sample* out_buf;
    const short int* in_buf;
    Sample r;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	out_buf = m_data[i];
	in_buf = src + i;

	for (j = 0; j < n_frames; ++j) {
	    r = (float) *in_buf / SHORT_MAX_VALUE_F;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf++ = r;
	    in_buf += m_info.num_channels;
	}
    }    
}

void AudioBuffer::deinterleaveI32(const int* src, size_t n_frames)
{
    size_t i, j;
    Sample* out_buf;
    const int* in_buf;
    Sample r;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	out_buf = m_data[i];
	in_buf = src + i;

	for (j = 0; j < n_frames; ++j) {
	    r = (float) *in_buf / INT_MAX_VALUE_F;
	    if (r < -1) r = -1; 
	    else if (r > 1) r = 1;
	    *out_buf++ = r;
	    in_buf += m_info.num_channels;
	}
    }
}

} /* namespace psynth */
