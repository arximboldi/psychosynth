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

#ifndef PSYNTH_FILTER_H
#define PSYNTH_FILTER_H

#include <libpsynth/common/AudioInfo.h>
#include <cmath>

namespace psynth
{

class FilterValues
{
    friend class Filter;
    
public:
    enum Type {
	LOWPASS,
	HIPASS,
	BANDPASS_CSG,
	BANDPASS_CZPG,
	NOTCH,
	MOOG
    };

private:
    float m_b0a0, m_b1a0, m_b2a0, m_a1a0, m_a2a0; // filter coeffs
    float m_r, m_p, m_k; // coeffs for moog-filter

    Type m_type;
    float m_freq;
    float m_res;
    float m_srate;
    
public:
    FilterValues(Type type = LOWPASS,
		 float freq = 220.0f,
		 float res = 0.1f,
		 float srate = 44100.0f) {
	calculate(type, freq, res, srate);
    }
    
    void calculate(Type type, float freq, float res, float srate);
    void calculate(float freq);
    void calculate();
    
    Type getType() {
	return m_type;
    };

    float getFrequency() {
	return m_freq;
    }

    float getResonance() {
	return m_res;
    }
};

class Filter
{
    FilterValues* m_coef;
    bool m_local_coef;
    /*
      in/out history
    */
    Sample m_ou1, m_ou2, m_in1, m_in2;
    
    /*
      in/out history for moog-filter
    */
    Sample m_y1, m_y2, m_y3, m_y4, m_oldx, m_oldy1, m_oldy2, m_oldy3;
public:
    Filter(FilterValues* coef = 0) :
	m_coef(coef), m_local_coef(false),
	m_ou1(0), m_ou2(0),
	m_in1(0), m_in2(0),
	m_y1(0), m_y2(0),
	m_y3(0), m_y4(0),
	m_oldx(0),
	m_oldy1(0), m_oldy2(0), m_oldy3(0) {
	if (!m_coef) {
	    m_coef = new FilterValues;
	    m_local_coef = true;
	}
    }

    Filter(const Filter& f) :
	m_coef(f.m_coef), m_local_coef(f.m_local_coef),
	m_ou1(f.m_ou1), m_ou2(f.m_ou2),
	m_in1(f.m_in1), m_in2(f.m_in2),
	m_y1(f.m_y1), m_y2(f.m_y2),
	m_y3(f.m_y3), m_y4(f.m_y4),
	m_oldx(f.m_oldx),
	m_oldy1(f.m_oldy1), m_oldy2(f.m_oldy2), m_oldy3(f.m_oldy3) {
	if (f.m_local_coef) {
	    m_coef = new FilterValues;
	    *m_coef = *f.m_coef;
	}
    }

    Filter& operator= (const Filter& f) {
	if (this != &f) {
	    m_coef = f.m_coef;
	    m_local_coef = f.m_local_coef;
	    m_ou1 = f.m_ou1;
	    m_ou2 = f.m_ou2;
	    m_in1 = f.m_in1;
	    m_in2 = f.m_in2;
	    m_y1 = f.m_y1;
	    m_y2 = f.m_y2;
	    m_y3 = f.m_y3;
	    m_y4 = f.m_y4;
	    m_oldx = f.m_oldx;
	    m_oldy1 = f.m_oldy1;
	    m_oldy2 = f.m_oldy2;
	    m_oldy3 = f.m_oldy3;

	    if (f.m_local_coef) {
		m_coef = new FilterValues;
		*m_coef = *f.m_coef;
	    }	    
	}

	return *this;
    }
    
    ~Filter() {
	if (m_local_coef)
	    delete m_coef;
    }
    
    FilterValues* getValues() {
	return m_coef;
    }

    /*void setValues(FilterValues* coef) {
	m_coef = coef;
	}*/
    
    Sample update(Sample x);
};

} /* namespace psynth */

#endif /* PSYNTH_FILTER_H */
