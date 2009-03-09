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
#include "base/misc.hpp"
#include "node/node_types.hpp"
#include "synth/envelope_multi.hpp"
#include "node/node_step_seq.hpp"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY(node_step_seq);

node_step_seq::node_step_seq(const audio_info& info) :
    node (info,
	  NODE_STEPSEQ,
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
    
    add_param ("bpm", node_param::FLOAT, &m_param_bpm);
    add_param ("shape", node_param::INT, &m_param_shape);
    add_param ("high", node_param::FLOAT, &m_param_high);
    add_param ("slope", node_param::FLOAT, &m_param_slope);
    add_param ("current_step", node_param::INT, &m_cur_step);
    add_param ("num_steps", node_param::INT, &m_param_num_steps);
	
    for (i = 0; i < MAX_STEPS; ++i) {
	m_param_step[i] = DEFAULT_STEP;
	add_param (string("step") + itoa(i, 10), node_param::INT, &m_param_step[i]);
    }

    set_output_stable_value (LINK_CONTROL, OUT_C_OUTPUT, 1.0f);
    init_envelope_values ();

    if (m_param_step[m_cur_step])
	m_env.set_values (&m_hi_env_vals);
    else
	m_env.set_values (&m_lo_env_vals);
}

void node_step_seq::do_update (const node* caller,
			       int caller_port_type,
			       int caller_port)
{
    sample_buffer* outbuf = get_output<sample_buffer> (LINK_CONTROL, OUT_C_OUTPUT);
    sample* output = outbuf->get_data();
    const sample_buffer* bpmbuf = get_input<sample_buffer> (LINK_CONTROL, IN_C_BPM);
    const sample* bpm = bpmbuf ? bpmbuf->get_data() : 0;
    int i;

    update_envelope_values ();
    
    for (i = 0; i < get_info ().block_size; ++i) {
	if (bpm)
	    update_envelope_factor (*bpm++);
	*output++ = m_env.update();
	
	if (m_env.finished ()) {
	    m_cur_step = (m_cur_step + 1) % m_param_num_steps;
	    m_env.press();
	    m_env.release();
	    if (m_param_step[m_cur_step])
		m_env.set_values (&m_hi_env_vals);
	    else
		m_env.set_values (&m_lo_env_vals);
	}
    }

    m_old_param_shape = m_param_shape;
    m_old_param_high = m_param_high;
}

void node_step_seq::do_advance()
{
}

/* TODO */
void node_step_seq::on_info_change ()
{
}

void node_step_seq::init_envelope_values ()
{
    m_lo_env_vals.resize(2);
    m_lo_env_vals[0] = env_point (0.0f, 0.0f);
    m_lo_env_vals[1] = env_point (1.0f, 0.0f);

    create_shape ();
    
    update_envelope_factor (0);
}

void node_step_seq::create_shape ()
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

    m_hi_env_vals[0] = env_point (0.0f, 0.0f);
    m_hi_env_vals[m_hi_env_vals.size()-1] = env_point (1.0f, 0.0f);
    
    update_shape ();
}

void node_step_seq::update_shape ()
{
    switch (m_param_shape) {
    case SHAPE_SQUARE:
	m_hi_env_vals[1] = env_point (m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = env_point (m_param_high - m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[3] = env_point (m_param_high, 0.0f);
	break;
    case SHAPE_TRIANGLE:
	m_hi_env_vals[1] = env_point (m_param_high/2.0f, 1.0f);
	m_hi_env_vals[2] = env_point (m_param_high, 0.0f);
	break;
    case SHAPE_FWSAWTOOTH:
	m_hi_env_vals[1] = env_point (m_param_high - m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = env_point (m_param_high, 0.0f);
	break;
    case SHAPE_BWSAWTOOTH:
	m_hi_env_vals[1] = env_point (m_param_slope * m_param_high, 1.0f);
	m_hi_env_vals[2] = env_point (m_param_high, 0.0f);
	break;
    default:
	break;
    }
}

void node_step_seq::update_envelope_values ()
{
    if (m_param_shape != m_old_param_shape) {
	create_shape ();
	m_env.restart ();
    }

    if (m_param_high != m_old_param_high) {
	update_shape ();
	m_env.set_time (m_env.get_time() * m_param_high/m_old_param_high);
    }
    
    update_envelope_factor (0);
}

void node_step_seq::update_envelope_factor (float mod)
{
    //cout << "m_param_bpm: " << m_param_bpm << endl;
    //cout << "mod: " << mod << endl;
    //cout << "sample_rate: " << get_info ().sample_rate;
    float factor =
	(m_param_bpm + m_param_bpm * mod) /
	(60.0f  * get_info ().sample_rate);
    m_lo_env_vals.set_factor (factor);
    m_hi_env_vals.set_factor (factor);
}

}
