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

class EnvelopeMulti;
class EnvelopeMultiValues;

class ObjectStepSeq : public Object
{
public:
    static const int MAX_STEPS = 16;
    static const float DEFAULT_BPM = 100.0;
    static const int DEFAULT_NUM_STEPS = 12;
    static const int DEFAULT_STEP = true;
    static const float DEFAULT_HIGH = 0.8;
    static const int DEFAULT_SLOPE = 0.1f;
    
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_BPM,
	N_IN_C_SOCKETS
    };

    enum ParamID {
	PARAM_BPM = Object::N_COMMON_PARAMS,
	PARAM_HIGH,
	PARAM_SLOPE,
	PARAM_NUM_STEPS,
	PARAM_STEP_0,
	PARAM_STEP_1,
	PARAM_STEP_2,
	PARAM_STEP_3,
	PARAM_STEP_4,
	PARAM_STEP_5,
	PARAM_STEP_6,
	PARAM_STEP_7,
	PARAM_STEP_8,
	PARAM_STEP_9,
	PARAM_STEP_10,
	PARAM_STEP_11,
	PARAM_STEP_12,
	PARAM_STEP_13,
	PARAM_STEP_14,
	PARAM_STEP_15,
	N_PARAM,
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

    float m_param_bpm;
    float m_param_high;
    float m_param_slope;
    int m_param_num_steps;
    int m_param_step[MAX_STEPS];
    
    EnvelopeMultiValues m_hi_env_vals;
    EnvelopeMultiValues m_lo_env_vals;
    EnvelopeMulti m_env;
    int m_cur_step;

    void initEnvelopeValues();
    void updateEnvelopeValues();
    void updateEnvelopeFactor(float mod);
public:
    ObjectStepSeq(const AudioInfo& info);   
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectStepSeq, "step_sequencer");

} /* namespace psynth */


#endif /* PSYNTH_OBJECTSTEPSEQ_H */
