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

#include "synth/audio_buffer.hpp"

#define INT_MAX_VALUE_F    2147483647.0
#define SHORT_MAX_VALUE_F  32766.0
#define CHAR_MAX_VALUE_F    128.0

using namespace std;

namespace psynth
{

void audio_buffer::allocate()
{
    int i;
    m_data  = new sample* [ m_info.num_channels ];
    *m_data = new sample  [ m_info.block_size * m_info.num_channels ];
    for (i = 1; i < m_info.num_channels; i++)
	m_data[i] = m_data[i-1] + m_info.block_size;
    memset(*m_data, 0, sizeof(sample) * m_info.num_channels * m_info.block_size);
}

void audio_buffer::liberate()
{
    delete [] *m_data;
    delete [] m_data;
}

audio_buffer& audio_buffer::operator= (const audio_buffer& buf)
{
    if (&buf != this) {
	if (buf.m_info.block_size != m_info.block_size || 
	    buf.m_info.num_channels != buf.m_info.num_channels) {
	    liberate();
	    m_info = buf.m_info;
	    allocate();
	} else
	    m_info.sample_rate = buf.m_info.sample_rate;

	memcpy(*m_data, *buf.m_data, sizeof(sample) * m_info.num_channels * m_info.block_size);
    }
    return *this;
}

void audio_buffer::reverse(int n_frames)
{
    int i;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	std::reverse(m_data[i], m_data[i] + n_frames);
    }    
}

void audio_buffer::interleave(sample* dest, size_t n_frames) const
{
    size_t i, j;
    sample* out_buf;
    const sample* in_buf;
    
    for (i = 0; i < m_info.num_channels; ++i) {
	in_buf = m_data[i];
	out_buf = dest + i;

	for (j = 0; j < n_frames; ++j) {
	    *out_buf = *in_buf++;
	    out_buf += m_info.num_channels;
	}
    }
}

void audio_buffer::interleave_c8(char* dest, size_t n_frames) const
{
    size_t i, j;
    char* out_buf;
    const sample* in_buf;
    sample r;
    
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

void audio_buffer::interleave_s16(short int* dest, size_t n_frames) const
{
    size_t i, j;
    short int* out_buf;
    const sample* in_buf;
    sample r;
    
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

void audio_buffer::interleave_i32(int* dest, size_t n_frames) const
{
    size_t i, j;
    int* out_buf;
    const sample* in_buf;
    sample r;

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

void audio_buffer::deinterleave(const sample* src, size_t n_frames, int num_chan)
{
    size_t i, j;
    sample* out_buf;
    const sample* in_buf;

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

void audio_buffer::deinterleave(const sample* src,
			       size_t start, size_t end, int num_chan)
{
    size_t i, j;
    sample* out_buf;
    const sample* in_buf;

    num_chan = min(m_info.num_channels, num_chan);
    
    for (i = 0; i < num_chan; ++i) {
	out_buf = m_data[i];
	in_buf = src + i;

	for (j = start; j < end; ++j) {
	    *out_buf++ = *in_buf;
	    in_buf += num_chan;
	}
    }

    for (; i < m_info.num_channels; ++i) {
	out_buf = m_data[i];
	in_buf = m_data[i - num_chan];

	for (j = start; j < end; ++j) {
	    *out_buf++ = *in_buf;
	    in_buf += num_chan;
	}
    }
}

void audio_buffer::deinterleave_c8(const char* src, size_t n_frames)
{
    size_t i, j;
    sample* out_buf;
    const char* in_buf;
    sample r;
    
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

void audio_buffer::deinterleave_s16(const short int* src, size_t n_frames)
{
    size_t i, j;
    sample* out_buf;
    const short int* in_buf;
    sample r;
    
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

void audio_buffer::deinterleave_i32(const int* src, size_t n_frames)
{
    size_t i, j;
    sample* out_buf;
    const int* in_buf;
    sample r;
    
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
