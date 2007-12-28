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

#ifndef OBJECTRADAR_H
#define OBJECTRADAR_H

#include "object/Object.h"

class ObjectRadar : public Object
{
    float m_param_speed;
    float m_param_angle;
    float m_angle;
    
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance();
    
public:
    enum {
      N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_SPEED,
	N_IN_C_SOCKETS
    };
	
    enum OutAudioSocketID {
	N_OUT_A_SOCKETS
    };
	
    enum OutControlSocketID {
	OUT_C_TRIGGER,
	N_OUT_C_SOCKETS
    };

    enum ParamID {
	PARAM_SPEED,
	PARAM_ANGLE,
	N_PARAM
    };
    
    ObjectRadar(const AudioInfo& info);
};

#endif /* OBJECTRADAR_H */
