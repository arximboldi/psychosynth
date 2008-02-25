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

#include "object/ObjectRadar.h"

using namespace std;

ObjectRadar::ObjectRadar(const AudioInfo& prop, int numchan) : 
    Object(prop,
	   OBJ_MIXER,
	   N_PARAM,
	   N_IN_A_SOCKETS, 
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_speed(0.5f),
    m_numchan(numchan)
{
    configureLocalParam(PARAM_AMPLITUDE, Object::PARAM_FLOAT, &m_param_ampl);
}

void ObjectRadar::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    if (caller) {
	Sample* out_buf = getOutput(LINK_CONTROL, OUT_C_TRIGGER)->getData();
	Vector2f pos;
	caller->getParam(PARAM_COMMON, PARAM_POSITION, pos);

	for (size_t i = 0; i < getInfo().block_size; ++i) {
	    new_angle += 1 / getInfo().sample_rate;
	    *out_buf++ = 
	}
    }
}

void ObjectRadar::doAdvance()
{
    m_angle += m_param_speed * getInfo().block_size / getInfo().sample_rate;
}

