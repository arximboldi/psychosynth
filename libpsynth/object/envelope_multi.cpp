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

#include <iostream>
#include "object/envelope_multi.h"

namespace psynth
{

using namespace std;

void envelope_multi_values::set_asr (env_point a, env_point s, env_point r)
{
    m_points.resize(4);
    m_points[0] = env_point (0, 0);
    m_points[1] = a;
    m_points[2] = s;
    m_points[3] = r;
    m_sustain = 2;
}

void envelope_multi_values::set_adsr (env_point a, env_point d, env_point s, env_point r)
{
    m_points.resize(5);
    m_points[0] = env_point (0, 0);
    m_points[1] = a;
    m_points[2] = d;
    m_points[3] = s;
    m_points[4] = r;
    m_sustain = 3;
}

/*
 * TODO: Implement sustain!
 */
float envelope_multi::update (float sample)
{
    float val;

    //cout << "factor: " << m_val->m_factor << endl;
    //cout << "prev m_time: " << m_time << endl; 

    m_time += sample * m_val->m_factor;

    //cout << "then m_time: " << m_time << endl;
    //cout << "prev m_cur_point: " << m_cur_point << endl;

    while (m_cur_point < m_val->size()-1 &&
	   m_val->point(m_cur_point+1).dt < m_time)
	++m_cur_point;

    //cout << "then m_cur_point: " << m_cur_point << endl;

    if (m_cur_point < m_val->size() - 1)
	//if (m_pressed && m_cur_point == m_val->m_sustain)
	//    val = m_val->point(m_val->m_sustain).val;
	//else
	    val =
		m_val->point(m_cur_point).val +
		(m_val->point(m_cur_point+1).val - m_val->point(m_cur_point).val) /
		(m_val->point(m_cur_point+1).dt - m_val->point(m_cur_point).dt) * 
		(m_time - m_val->point(m_cur_point).dt);
    else if (m_cur_point == m_val->size() - 1) {
	val = m_val->point(m_val->size()-1).val;
	m_time = 0.0f;
    }

    return val;
}



} /* namespace psynth */
