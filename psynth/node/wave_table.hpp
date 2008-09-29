/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#ifndef PSYNTH_WAVETABLE_H
#define PSYNTH_WAVETABLE_H

#include <cmath>

#include <psynth/common/audio_buffer.hpp>
#include <psynth/common/misc.hpp>

namespace psynth
{

class wave_table
{
public:
    typedef sample(*wave_func_t)(float);
    
private:
    sample* m_table;
    size_t m_size;
    
public:
    wave_table () : m_table(NULL), m_size(0) {}

    wave_table (size_t size) :
	m_size(size) {
	m_table = new sample[size];
    }

    wave_table (size_t size, wave_func_t func) :
	m_size(size) {
	m_table = new sample[size];
	fill(func);
    }
    
    wave_table (const wave_table& wave) :
	m_size(wave.m_size) {
	m_table = new sample[m_size];
	memcpy(m_table, wave.m_table, sizeof(sample) * m_size);
    }
    
    ~wave_table () {
	delete [] m_table;
    }

    wave_table& operator= (const wave_table& wave) {
	if (&wave != this) {
	    if (wave.m_size != m_size) {
		delete m_table;
		m_size = wave.m_size;
		m_table = new sample[m_size];
	    }
	    
	    memcpy(m_table, wave.m_table, sizeof(sample) * m_size);
	}
	return *this;
    }
    
    sample get (float x) const {
	float  findex = (m_size - 1) * phase(x);
	size_t index  = findex;
	float alpha = findex - index;
	
	sample out = m_table[index];
	out += alpha * (m_table[index + 1 >= m_size ? 0 : index + 1] - out);

	return out;
    }

    void fill (size_t new_size, wave_func_t func) {
	if (m_size != new_size) {
	    m_size = new_size;
	    delete [] m_table;
	    m_table = new sample[new_size];
	}
	fill(func);
    }

    void fill(wave_func_t func);
};

} /* namespace psynth */

#endif /* PSYNTH_WAVETABLE_H */
