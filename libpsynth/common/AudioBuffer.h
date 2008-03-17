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

#ifndef PSYNTH_AUDIOBUFFER_H
#define PSYNTH_AUDIOBUFFER_H

#include <cstring>

#include <libpsynth/common/AudioInfo.h>

namespace psynth
{

class AudioBuffer
{
    AudioInfo m_info;
    Sample** m_data;

    void allocate();
	
    void liberate();

public:
    AudioBuffer() :
	m_info(0,0,0),
	m_data(NULL) {}
	
    AudioBuffer(const AudioInfo& info) : 
	m_info(info) {
	allocate();
    }
	
    AudioBuffer(const AudioBuffer& buf) :
	m_info(buf.m_info) {
	allocate();
	memcpy(*m_data, *buf.m_data, sizeof(Sample) * m_info.num_channels * m_info.block_size);
    }
	
    ~AudioBuffer() {
	liberate();
    }
	
    AudioBuffer& operator= (const AudioBuffer& buf);

    void interleave(Sample* dest, size_t n_frames) const;
    void interleaveC8(char* dest, size_t n_frames) const;
    void interleaveS16(short int* dest, size_t n_frames) const;
    void interleaveI32(int* dest, size_t n_frames) const;

    void deinterleave(const Sample* src, size_t n_frames);
    void deinterleaveC8(const char* src, size_t n_frames);
    void deinterleaveS16(const short int* src, size_t n_frames);
    void deinterleaveI32(const int* src, size_t n_frames);

    
    const AudioInfo& getInfo() const {
	return m_info;
    }
	
    Sample* getChannel(int n) {
	return m_data[n];
    }
	
    const Sample* getChannel(int n) const {
	return m_data[n];
    }
	
    Sample** getData() {
	return m_data;
    }
	
    Sample** const getData() const {
	return m_data;
    }
    
    Sample* operator[] (int n) {
	return m_data[n];
    }
	
    const Sample* operator[] (int n) const {
	return m_data[n];
    }

    size_t size() const {
	return m_info.block_size;
    }
    
    void resize(size_t new_size) {
	liberate();
	m_info.block_size = new_size;
	allocate();
    }
    
    void zero() {
	memset(*m_data, 0, sizeof(Sample) * m_info.block_size * m_info.num_channels);
    }

    void setInfo(const AudioInfo& info) {
	if (m_info.block_size != info.block_size ||
	    m_info.num_channels != info.num_channels) {
	    liberate();
	    m_info = info;
	    allocate();
	} else
	    m_info = info;	    
    }
};

} /* namespace psynth */

#endif /* PSYNTH_AUDIOBUFFER_H */

