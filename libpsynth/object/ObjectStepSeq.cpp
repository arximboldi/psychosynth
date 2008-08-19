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

#include <algorithm>
#include "common/misc.h"
#include "object/KnownObjects.h"
#include "object/EnvelopeMulti.h"
#include "object/ObjectStepSeq.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectStepSeq);

ObjectStepSeq::ObjectStepSeq(const audio_info& info) :
    Object(info,
	   OBJ_STEPSEQ,
	   "stepseq",
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_bpm(DEFAULT_BPM),
    m_param_shape(SHAPE_SQUARE),
    m_param_high(DEFAULT_HIGH),
    m_param_slope(DEFAULT_SLOPE),
    m_param_num_steps(DEFAULT_NUM_STEPS),
    m_env(&m_hi_env_vals),
    m_cur_step(0)
{
    int i;
    
    addParam("bpm", ObjParam::FLOAT, &m_param_bpm);
    addParam("shape", ObjParam::INT, &m_param_shape);
    addParam("high", ObjParam::FLOAT, &m_param_high);
    addParam("slope", ObjParam::FLOAT, &m_param_slope);
    addParam("current_step", ObjParam::INT, &m_cur_step);
    addParam("num_steps", ObjParam::INT, &m_param_num_steps);
	
    for (i = 0; i < MAX_STEPS; ++i) {
	m_param_step[i] = DEFAULT_STEP;
	addParam(string("step") + itoa(i, 10), ObjParam::INT, &m_param_step[i]);
    }

    setOutputStableValue(LINK_CONTROL, OUT_C_OUTPUT, 1.0f);
    initEnvelopeValues();

    if (m_param_step[m_cur_step])
	m_env.setValues(&m_hi_env_vals);
    else
	m_env.setValues(&m_lo_env_vals);
}

void ObjectStepSeq::doUpdate(const Object* caller,
			     int caller_port_type,
			     int caller_port)
{
    sample_buffer* outbuf = getOutput<sample_buffer>(LINK_CONTROL, OUT_C_OUTPUT);
    sample* output = outbuf->get_data();
    const sample_buffer* bpmbuf = getInput<sample_buffer>(LINK_CONTROL, IN_C_BPM);
    const sample* bpm = bpmbuf ? bpmbuf->get_data() : 0;
    int i;

    updateEnvelopeValues();
    
    for (i = 0; i < getInfo().block_size; ++i) {
	if (bpm)
	    updateEnvelopeFactor(*bpm++);
	*output++ = m_env.update();
	
	if (m_env.finished ()) {
	    m_cur_step = (m_cur_step + 1) % m_param_num_steps;
	    m_env.press();
	    m_env.release();
	    if (m_param_step[m_cur_step])
		m_env.setValues (&m_hi_env_vals);
	    else
		m_env.setValues (&m_lo_env_vals);
	}
    }

    m_old_param_shape = m_param_shape;
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

    createShape();
    
    updateEnvelopeFactor(0);
}

void ObjectStepSeq::createShape()
{
    switch (m_param_shape) {
    case SHAPE_SQUARE:
	m_hi_env_vals.resize(5);
	break;
    case SHAPE_TRIANGLE:
    case SHAPE_FWSAWTOOTH:
    case SHAPE_BWSAWTOOTH:
	m_hi_env_vals.resize(4);
	break;
    default:
	break;
    }

    m_hi_env_vals[0] = EnvPoint(0.0f, 0.0f);
    m_hi_env_vals[m_hi_env_vals.size()-1] = EnvPoint(1.0f, 0.0f);
    
    updateShape();
}

void ObjectStepSeq::updateShape()
{
    switch (m_param_shape) {
    case SHAPE_SQUARE:
	m_hi_env_vals[1] = EnvPoint(m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = EnvPoint(m_param_high - m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[3] = EnvPoint(m_param_high, 0.0f);
	break;
    case SHAPE_TRIANGLE:
	m_hi_env_vals[1] = EnvPoint(m_param_high/2.0f, 1.0f);
	m_hi_env_vals[2] = EnvPoint(m_param_high, 0.0f);
	break;
    case SHAPE_FWSAWTOOTH:
	m_hi_env_vals[1] = EnvPoint(m_param_high - m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = EnvPoint(m_param_high, 0.0f);
	break;
    case SHAPE_BWSAWTOOTH:
	m_hi_env_vals[1] = EnvPoint(m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = EnvPoint(m_param_high, 0.0f);
	break;
    default:
	break;
    }
}

void ObjectStepSeq::updateEnvelopeValues()
{
    if (m_param_shape != m_old_param_shape) {
	createShape();
	m_env.restart();
    }

    if (m_param_high != m_old_param_high) {
	updateShape();
	m_env.setTime(m_env.getTime() * m_param_high/m_old_param_high);
    }
    
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
