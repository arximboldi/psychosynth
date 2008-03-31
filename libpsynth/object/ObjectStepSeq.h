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
#include <libpsynth/object/EnvelopeMulti.h>

namespace psynth
{

class ObjectStepSeq : public Object
{
public:
    static const int MAX_STEPS = 32;
    static const float DEFAULT_BPM = 260.0;
    static const int DEFAULT_NUM_STEPS = 12;
    static const int DEFAULT_STEP = true;
    static const float DEFAULT_HIGH = 0.8f;
    static const float DEFAULT_SLOPE = 0.1f;

    enum Shape {
	SHAPE_SQUARE,
	SHAPE_TRIANGLE,
	SHAPE_FWSAWTOOTH,
	SHAPE_BWSAWTOOTH,
	N_SHAPES
    };
    
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_BPM,
	N_IN_C_SOCKETS
    };

    enum ParamID {
	PARAM_BPM = Object::N_COMMON_PARAMS,
	PARAM_SHAPE,
	PARAM_HIGH,
	PARAM_SLOPE,
	PARAM_CURRENT_STEP,
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
	PARAM_STEP_16,
	PARAM_STEP_17,
	PARAM_STEP_18,
	PARAM_STEP_19,
	PARAM_STEP_20,
	PARAM_STEP_21,
	PARAM_STEP_22,
	PARAM_STEP_23,
	PARAM_STEP_24,
	PARAM_STEP_25,
	PARAM_STEP_26,
	PARAM_STEP_27,
	PARAM_STEP_28,
	PARAM_STEP_29,
	PARAM_STEP_30,
	PARAM_STEP_31,
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
    void createShape();
    void updateShape();
    
    float m_param_bpm;
    int m_param_shape;
    float m_param_high;
    float m_param_slope;
    int m_param_num_steps;
    int m_param_step[MAX_STEPS];

    float m_old_param_high;
    int m_old_param_shape;
    
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

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectStepSeq, "stepseq");

} /* namespace psynth */


#endif /* PSYNTH_OBJECTSTEPSEQ_H */
