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

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <cstring>

#include "common/AudioInfo.h"
#include "psychosynth.h"

class AudioBuffer {
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
    
    void clear() {
	memset(*m_data, 0, sizeof(Sample) * m_info.block_size * m_info.num_channels);
    }
};

#endif /* AUDIOBUFFER_H */

