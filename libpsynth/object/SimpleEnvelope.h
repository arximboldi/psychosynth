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

#ifndef PSYNTH_SIMPLEENVELOPE_H
#define PSYNTH_SIMPLEENVELOPE_H

/**
 * Simplistic evenlope implementation to soften object connects/disconnects.
 */
namespace psynth
{

class SimpleEnvelope
{
    float m_rise_dt;
    float m_fall_dt;
    float m_curr_dt;
    float m_val;
    
public:
    SimpleEnvelope() :
	m_rise_dt(0.0f),
	m_fall_dt(0.0f),
	m_curr_dt(0.0f),
	m_val(0.0f)
	{}
    
    SimpleEnvelope(float rise_dt, float fall_dt) :
	m_rise_dt(rise_dt),
	m_fall_dt(fall_dt),
	m_curr_dt(0.0f),
	m_val(0.0f)
	{}

    float setDeltas(float rise_dt, float fall_dt) {
	m_rise_dt = rise_dt;
	m_fall_dt = fall_dt;
    }

    void set(float value) {
	m_val = value;
    }
    
    float update() {
	float val = m_val;
	m_val = m_val + m_curr_dt;
	if (m_val > 1.0f) m_val = 1.0;
	else if (m_val < 0.0f) m_val = 0.0;
	return val;
    }

    float update(float sample) {
	float val = m_val;
	m_val = m_val + m_curr_dt * sample;
	if (m_val > 1.0f) m_val = 1.0;
	else if (m_val < 0.0f) m_val = 0.0;
	return val;
    }

    float update(float* samples, int n_samples) {
	while(n_samples--)
	    *samples++ *= update();
    }

    void press() {
	m_curr_dt = m_rise_dt;
    }

    void release() {
	m_curr_dt = m_fall_dt;
    }

    bool finished() {
	return m_val <= 0.0f;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_SIMPLEENVELOPE_H */
