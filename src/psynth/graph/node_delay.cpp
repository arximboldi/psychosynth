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
#include "graph/node_delay.hpp"
#include "graph/node_types.hpp"

using namespace std;

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_NODE_FACTORY(node_delay);

node_delay::node_delay(const audio_info& prop) : 
    node (prop,
	  NODE_DELAY,
	  "delay",
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_param_delay(DEFAULT_DELAY),
    m_param_depth(DEFAULT_DEPTH),
    m_pos(0),
    m_max_delay_pos(prop.sample_rate * MAX_DELAY * 2.0f),
    m_buffer(m_max_delay_pos, audio_frame (0), 0)
{
    add_param ("delay", node_param::FLOAT, &m_param_delay);
    add_param ("depth", node_param::FLOAT, &m_param_depth);
}

void node_delay::on_info_change()
{
    m_max_delay_pos = get_info().sample_rate * MAX_DELAY * 2.0f;
    m_buffer.recreate (m_max_delay_pos, audio_frame (0), 0);
}

node_delay::~node_delay()
{
}

int node_delay::do_update_channel (int chan)
{
    const audio_buffer* _input = get_input<audio_buffer>(LINK_AUDIO, IN_A_INPUT);
    const sample_buffer* _delay = get_input<sample_buffer>(LINK_CONTROL, IN_C_DELAY);
    const sample_buffer* _depth = get_input<sample_buffer>(LINK_CONTROL, IN_C_DEPTH);
    audio_buffer* _output = get_output<audio_buffer>(LINK_AUDIO, IN_A_INPUT);    

    const sample* in_buf = _input ? (const sample*)&const_range (*_input)[0][chan] : 0;
    const sample* dep_buf = _depth ? (const sample*)&const_range (*_depth)[0] : 0;
    const sample* del_buf = _delay ? (const sample*)&const_range (*_delay)[0] : 0;
    sample* out_buf = (sample*)&range (*_output)[0][chan];
    sample* tmp_buf = (sample*)&range (m_buffer)[0][chan];

    link_envelope in_env = get_in_envelope (LINK_AUDIO, IN_A_INPUT);
    
    float delay, the_delay;
    size_t i;
    int   pos = m_pos;
    float prev, next, frac;
    float delayed_sample;
    float in_val;
    
    delay = m_param_delay * get_info().sample_rate;
    for (i = 0; i < get_info().block_size; ++i) {
	if (in_buf)
	    in_val = *in_buf++ * (float) (audio_sample) in_env.update();
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

void node_delay::do_update (const node* caller,
			     int caller_port_type, int caller_por)
{
    /*
      if (m_param_delay != m_old_param_delay)
      m_buffer.zero();
      m_old_param_delay = m_param_delay;
    */
    
    int new_pos = m_pos;
 
    for (size_t i = 0; i < get_info().num_channels; ++i)
	new_pos = do_update_channel (i);
    
    m_pos = new_pos;
}

} /* namespace graph */
} /* namespace psynth */

