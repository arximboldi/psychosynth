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

#ifndef PSYNTH_OBJECTSTEPSEQ_H
#define PSYNTH_OBJECTSTEPSEQ_H

#include <libpsynth/object/Object.h>
#include <libpsynth/object/ObjectFactory.h>

namespace psynth
{

class ObjectStepSeq : public Object
{
public:
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_RATE,
	N_IN_C_SOCKETS
    };

    enum ParamID {
	PARAM_SPEED = Object::N_COMMON_PARAMS,
	PARAM_NUM_STEPS,
	N_PARAM
    };
    
    enum OutAudioSocketID {
	N_OUT_A_SOCKETS
    };

    enum OutControlSocketID {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };
    
private:
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance();
    void onInfoChange();
    
public:
    ObjectStepSeq(const AudioInfo& info);   
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectStepSeq, "step_sequencer");

} /* namespace psynth */


#endif /* PSYNTH_OBJECTSTEPSEQ_H */
