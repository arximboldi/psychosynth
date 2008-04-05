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

#ifndef PSYNTH_OBJECTDELAY_H
#define PSYNTH_OBJECTDELAY_H

#include <vector>

#include <libpsynth/object/Object.h>
#include <libpsynth/object/ObjectFactory.h>

namespace psynth
{

class ObjectDelay : public Object
{
public:	
    enum InAudioSocketID {
	IN_A_INPUT,
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_DELAY,
	IN_C_DEPTH,
	N_IN_C_SOCKETS
    };
	
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };
	
    enum OutControlSocketID {
	N_OUT_C_SOCKETS
    };

    enum ParamID {
	PARAM_DELAY = Object::N_COMMON_PARAMS,
	PARAM_DEPTH,
	N_PARAM
    };

    static const float MAX_DELAY = 0.1f;
    static const float DEFAULT_DELAY = 0.0015f;
    static const float DEFAULT_DEPTH = 0.5f;
    
private:
    float m_param_delay;
    float m_param_depth;
    int m_max_delay_pos;
    int m_pos;
    
    AudioBuffer m_buffer;

    int doUpdateChannel(int chan);
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void onInfoChange();
    void doAdvance() {}
    
public:
    ObjectDelay(const AudioInfo& prop);
    ~ObjectDelay();
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectDelay, "delay");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTDELAY_H */

