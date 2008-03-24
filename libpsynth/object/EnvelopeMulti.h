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

#ifndef PSYNTH_ENVELOPEMULTI_H
#define PSYNTH_ENVELOPEMULTI_H

#include <vector>
#include <libpsynth/object/Envelope.h>

namespace psynth
{

struct EnvPoint
{
    int dt;
    int val;

    EnvPoint() {};
    EnvPoint(int _dt, int _val) :
	dt(_dt),
	val(_val)
	{}
};

class EnvelopeMultiValues
{
    friend class EnvelopeMulti;
    
    std::vector<EnvPoint> m_points;
    int m_sustain;
    
public:

    void setASR(EnvPoint a, EnvPoint s, EnvPoint r);
    void setADSR(EnvPoint a, EnvPoint d, EnvPoint s, EnvPoint r);

    void set(const std::vector<EnvPoint>& p, int sustain) {
	m_points = p;
	m_sustain = sustain;
    }
    
    EnvPoint& point(int index) {
	return m_points[index];
    }

    const EnvPoint& point(int index) const {
	return m_points[index];
    }

    EnvPoint& operator[](int index) {
	return m_points[index];
    }

    const EnvPoint& operator[](int index) const {
	return m_points[index];
    }

    int size() const {
	return m_points.size();
    }
};

class EnvelopeMulti : public Envelope
{
    EnvelopeMultiValues* m_val;
    int m_cur_point;
    float m_time;
    bool m_pressed;
     
public:
    EnvelopeMulti(EnvelopeMultiValues* val) :
	m_val(NULL),
	m_cur_point(0),
	m_time(0.0f),
	m_pressed(false)
	{}
    
    void press() {
	if (finished()) {
	    m_cur_point = 0;
	    m_time = 0;
	}
	m_pressed = true;
    }

    void release() {
	m_pressed = false;
    }

    float update();
    
    float update(float sample);
    
    void update(float* samples, int n_samples) {
	while(n_samples--)
	    *samples++ = update();
    }

    bool finished() {
	return m_cur_point >= m_val->size();
    }
    
};

} /* namespace psynth */

#endif /* PSYNTH_ENVELOPEMULTI_H */
