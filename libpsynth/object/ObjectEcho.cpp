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

#include "object/ObjectEcho.h"
#include "object/KnownObjects.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectEcho);

ObjectEcho::ObjectEcho(const AudioInfo& prop) : 
    Object(prop,
	   OBJ_ECHO,
	   "echo",
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_delay(DEFAULT_DELAY),
    m_old_param_delay(DEFAULT_DELAY),
    m_param_feedback(DEFAULT_FEEDBACK),
    m_param_hidamp(DEFAULT_HIDAMP),
    m_pos(0),
    m_old_val(prop.num_channels, 0.0f),
    m_buffer(prop, prop.sample_rate * MAX_DELAY * 2.0f)
{
    addParam("delay", ObjParam::FLOAT, &m_param_delay);
    addParam("feedback", ObjParam::FLOAT, &m_param_feedback);

    m_buffer.zero();
}

void ObjectEcho::onInfoChange()
{
    m_old_val.resize(getInfo().num_channels, 0.0f);
    m_buffer.resize(getInfo().sample_rate * MAX_DELAY * 2.0f);
    m_buffer.zero();
}

ObjectEcho::~ObjectEcho()
{
}

int ObjectEcho::doUpdateChannel(int chan)
{
    const AudioBuffer* _input = getInput<AudioBuffer>(LINK_AUDIO, IN_A_INPUT);
    const ControlBuffer* _delay = getInput<ControlBuffer>(LINK_CONTROL, IN_C_DELAY);
    const ControlBuffer* _feedback = getInput<ControlBuffer>(LINK_CONTROL, IN_C_FEEDBACK);
    AudioBuffer* _output = getOutput<AudioBuffer>(LINK_AUDIO, IN_A_INPUT);    

    const Sample* in_buf = _input ? _input->getChannel(chan) : NULL;
    const Sample* fb_buf = _feedback ? _feedback->getData() : NULL;
    const Sample* del_buf = _delay ? _delay->getData() : NULL;
    Sample* out_buf = _output->getChannel(chan);
    Sample* tmp_buf = m_buffer.getChannel(chan);

    EnvelopeSimple in_env = getInEnvelope(LINK_AUDIO, IN_A_INPUT);
    
    float delay;
    float in_val;
    int i;
    int pos = m_pos;
    float val;
  
    delay = m_param_delay * getInfo().sample_rate;
    for (i = 0; i < getInfo().block_size; ++i) {
	if (pos > delay)
	    pos = 0;
	
	in_val = in_buf ? *in_buf++ * in_env.update() : 0;
	val = tmp_buf[pos];

	//*out_buf++ = val;	
	if (fb_buf)
	    val = in_val - val * (m_param_feedback + m_param_feedback * *fb_buf++);
	else
	    val = in_val - val * m_param_feedback;
	*out_buf++ = val;

	/* Low pass filter. */
	val = val * m_param_hidamp + m_old_val[chan] * (1.0 - m_param_hidamp);
	tmp_buf[pos] = val;
	m_old_val[chan] = val;
	pos++;
    }

    return pos;
}

void ObjectEcho::doUpdate(const Object* caller, int caller_port_type, int caller_por)
{
    if (m_param_delay != m_old_param_delay)
	m_buffer.zero();
    m_old_param_delay = m_param_delay;

    int new_pos = m_pos;
 
    for (int i = 0; i < getInfo().num_channels; ++i)
	new_pos = doUpdateChannel(i);
    
    m_pos = new_pos;
}

}

