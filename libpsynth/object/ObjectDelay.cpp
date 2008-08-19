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

#include <algorithm>
#include "object/ObjectDelay.h"
#include "object/KnownObjects.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectDelay);

ObjectDelay::ObjectDelay(const audio_info& prop) : 
    Object(prop,
	   OBJ_DELAY,
	   "delay",
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_delay(DEFAULT_DELAY),
    m_param_depth(DEFAULT_DEPTH),
    m_pos(0),
    m_max_delay_pos(prop.sample_rate * MAX_DELAY * 2.0f),
    m_buffer(prop, m_max_delay_pos)
{
    addParam("delay", ObjParam::FLOAT, &m_param_delay);
    addParam("depth", ObjParam::FLOAT, &m_param_depth);

    m_buffer.zero();
}

void ObjectDelay::onInfoChange()
{
    m_max_delay_pos = getInfo().sample_rate * MAX_DELAY * 2.0f;
    m_buffer.resize(m_max_delay_pos);
    m_buffer.zero();
}

ObjectDelay::~ObjectDelay()
{
}

int ObjectDelay::doUpdateChannel(int chan)
{
    const audio_buffer* _input = getInput<audio_buffer>(LINK_AUDIO, IN_A_INPUT);
    const sample_buffer* _delay = getInput<sample_buffer>(LINK_CONTROL, IN_C_DELAY);
    const sample_buffer* _depth = getInput<sample_buffer>(LINK_CONTROL, IN_C_DEPTH);
    audio_buffer* _output = getOutput<audio_buffer>(LINK_AUDIO, IN_A_INPUT);    

    const sample* in_buf = _input ? _input->get_channel(chan) : 0;
    const sample* dep_buf = _depth ? _depth->get_data() : 0;
    const sample* del_buf = _delay ? _delay->get_data() : 0;
    sample* out_buf = _output->get_channel(chan);
    sample* tmp_buf = m_buffer.get_channel(chan);

    EnvelopeSimple in_env = getInEnvelope(LINK_AUDIO, IN_A_INPUT);
    
    float delay, the_delay;
    int i;
    int pos = m_pos;
    float prev, next, frac;
    float delayed_sample;
    float in_val;
    
    delay = m_param_delay * getInfo().sample_rate;
    for (i = 0; i < getInfo().block_size; ++i) {
	if (in_buf)
	    in_val = *in_buf++ * in_env.update();
	else
	    in_val = 0;
	
	tmp_buf[pos++] = in_val;
	if (pos > m_max_delay_pos)
	    pos = 0;

	if (del_buf)
	    the_delay = delay + delay * *del_buf++;
	else
	    the_delay = delay;

	prev = tmp_buf[(pos-(int)the_delay + m_max_delay_pos-1) % m_max_delay_pos];
        next = tmp_buf[(pos-(int)the_delay + m_max_delay_pos)   % m_max_delay_pos];
        frac = the_delay - floor(the_delay);
        delayed_sample = prev + frac * (next - prev);

	if (dep_buf)
	    *out_buf++ = in_val +
		(m_param_depth + m_param_depth * *dep_buf++) * delayed_sample;
	else
	    *out_buf++ = in_val + m_param_depth * delayed_sample;
    }

    return pos;
}

void ObjectDelay::doUpdate(const Object* caller, int caller_port_type, int caller_por)
{
    /*
    if (m_param_delay != m_old_param_delay)
	m_buffer.zero();
    m_old_param_delay = m_param_delay;
    */
    
    int new_pos = m_pos;
 
    for (int i = 0; i < getInfo().num_channels; ++i)
	new_pos = doUpdateChannel(i);
    
    m_pos = new_pos;
}

}

