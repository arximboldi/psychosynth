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

#ifndef PSYNTH_ENVELOPE_MULTI_H
#define PSYNTH_ENVELOPE_MULTI_H

#include <vector>
#include <libpsynth/node/envelope.h>

namespace psynth
{

struct env_point
{
    float dt;
    float val;

    env_point () {};
    env_point (float _dt, float _val)
	: dt(_dt)
	, val(_val)
	{}
};

class envelope_multi_values
{
    friend class envelope_multi;
    
    std::vector<env_point> m_points;
    int m_sustain;
    float m_factor;
    
public:

    envelope_multi_values() :
	m_sustain(1),
	m_factor(1.0f)
	{}
    
    void set_asr (env_point a, env_point s, env_point r);
    void set_adsr (env_point a, env_point d, env_point s, env_point r);

    void set_factor (float factor) {
	m_factor = factor;
    }

    void set_sustain (int sustain) {
	m_sustain = sustain;
    }
    
    void set (const std::vector<env_point>& p, int sustain) {
	m_points = p;
	m_sustain = sustain;
    }
    
    env_point& point (int index) {
	return m_points[index];
    }

    const env_point& point (int index) const {
	return m_points[index];
    }

    env_point& operator[] (int index) {
	return m_points[index];
    }

    const env_point& operator[] (int index) const {
	return m_points[index];
    }

    int size() const {
	return m_points.size();
    }

    void resize (size_t size) {
	m_points.resize(size);
    }
};

class envelope_multi : public envelope
{
    envelope_multi_values* m_val;
    int m_cur_point;
    float m_time;
    bool m_pressed;
     
public:
    envelope_multi (envelope_multi_values* val) :
	m_val(val),
	m_cur_point(0),
	m_time(0.0f),
	m_pressed(false)
	{}

    void set_values (envelope_multi_values* vals) {
	m_val = vals;
    }

    envelope_multi_values* get_values () {
	return m_val;
    }

    float get_time () const {
	return m_time;
    }

    float set_time (float time) {
	m_time = time;
    }

    void restart () {
	m_time = 0;
	m_cur_point = 0;
    }
    
    void press () {
	if (finished())
	    restart();
	m_pressed = true;
    }

    void release () {
	m_pressed = false;
    }

    float update () {
	return update(1.0f);
    }
    
    float update (float sample);
    
    void update (float* samples, int n_samples) {
	while (n_samples--)
	    *samples++ = update();
    }

    bool finished () {
	return m_cur_point >= m_val->size() - 1;
    }
    
};

} /* namespace psynth */

#endif /* PSYNTH_ENVELOPEMULTI_H */
