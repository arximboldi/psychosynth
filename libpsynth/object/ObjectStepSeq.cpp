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

#include "common/Misc.h"
#include "object/KnownObjects.h"
#include "object/EnvelopeMulti.h"
#include "object/ObjectStepSeq.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectStepSeq);

ObjectStepSeq::ObjectStepSeq(const AudioInfo& info) :
    Object(info,
	   OBJ_STEPSEQ,
	   "stepseq",
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_bpm(DEFAULT_BPM),
    m_param_high(DEFAULT_HIGH),
    m_param_slope(DEFAULT_SLOPE),
    m_param_num_steps(DEFAULT_NUM_STEPS),
    m_env(&m_hi_env_vals),
    m_cur_step(0)
{
    int i;
    
    addParam("bpm", ObjParam::FLOAT, &m_param_bpm);
    addParam("high", ObjParam::FLOAT, &m_param_high);
    addParam("slope", ObjParam::FLOAT, &m_param_slope);
    addParam("num_steps", ObjParam::INT, &m_param_num_steps);

    for (i = 0; i < MAX_STEPS; ++i) {
	m_param_step[i] = DEFAULT_STEP;
	addParam(string("step") + itoa(i, 10), ObjParam::INT, &m_param_step[i]);
    }

    initEnvelopeValues();
}

void ObjectStepSeq::doUpdate(const Object* caller,
			     int caller_port_type,
			     int caller_port)
{
    ControlBuffer* outbuf = getOutput<ControlBuffer>(LINK_CONTROL, OUT_C_OUTPUT);
    Sample* output = outbuf->getData();
    const ControlBuffer* bpmbuf = getInput<ControlBuffer>(LINK_CONTROL, IN_C_BPM);
    const Sample* bpm = bpmbuf ? bpmbuf->getData() : 0;
    int i;

    updateEnvelopeValues();
    
    for (i = 0; i < getInfo().block_size; ++i) {
	if (bpm)
	    updateEnvelopeFactor(*bpm++);
	*output++ = m_env.update();

	//cout << *(output-1) << endl;
	
	if (m_env.finished()) {
	    m_cur_step = (m_cur_step + 1) % m_param_num_steps;
	    if (m_param_step[m_cur_step])
		m_env.setValues(&m_hi_env_vals);
	    else
		m_env.setValues(&m_lo_env_vals);
	    m_env.press();
	    m_env.release();
	}
    }

    m_old_param_high = m_param_high;
}

void ObjectStepSeq::doAdvance()
{
}

void ObjectStepSeq::onInfoChange()
{
}

void ObjectStepSeq::initEnvelopeValues()
{
    m_lo_env_vals.resize(2);
    m_lo_env_vals[0] = EnvPoint(0.0f, 0.0f);
    m_lo_env_vals[1] = EnvPoint(1.0f, 0.0f);

    m_hi_env_vals.resize(5);
    m_hi_env_vals[0] = EnvPoint(0.0f, 0.0f);
    m_hi_env_vals[1] = EnvPoint(m_param_slope * m_param_high, 1.0f);
    m_hi_env_vals[2] = EnvPoint(m_param_high - m_param_slope * m_param_high, 1.0f);
    m_hi_env_vals[3] = EnvPoint(m_param_high, 0.0f);
    m_hi_env_vals[4] = EnvPoint(1.0f, 0.0f);

    cout << "m_param_high " << m_param_high << endl;
    cout << "m_param_slope " << m_param_slope << endl;
    
    for (int i = 0; i < 5; ++i)
	cout << m_hi_env_vals[i].dt << ", " << m_hi_env_vals[i].val << endl;

    updateEnvelopeFactor(0);
}

void ObjectStepSeq::updateEnvelopeValues()
{
    m_hi_env_vals[1] = EnvPoint(m_param_slope * m_param_high, 1.0f);
    m_hi_env_vals[2] = EnvPoint(m_param_high - m_param_slope * m_param_high, 1.0f);
    m_hi_env_vals[3] = EnvPoint(m_param_high, 0.0f);    

    if (m_param_step[m_cur_step])
	m_env.setValues(&m_hi_env_vals);
    else
	m_env.setValues(&m_lo_env_vals);

    m_env.setTime(m_env.getTime() * m_param_high/m_old_param_high);
    
    updateEnvelopeFactor(0);
}

void ObjectStepSeq::updateEnvelopeFactor(float mod)
{
    //cout << "m_param_bpm: " << m_param_bpm << endl;
    //cout << "mod: " << mod << endl;
    //cout << "sample_rate: " << getInfo().sample_rate;
    float factor =
	(m_param_bpm + m_param_bpm * mod) /
	(60.0f  * getInfo().sample_rate);
    m_lo_env_vals.setFactor(factor);
    m_hi_env_vals.setFactor(factor);
}

}
