/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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
#include <iostream>

#include "node/node_types.hpp"
#include "node/node_oscillator.hpp"

using namespace std;

namespace psynth
{

node_oscillator::node_oscillator (const audio_info& prop,
				  int obj_type,
				  const std::string& name,
				  int n_audio_out,
				  int n_control_out) : 
    node (prop,
	  obj_type,
	  name,
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  n_audio_out,
	  n_control_out),
    m_oscillator (prop),
    m_param_wave (OSC_SINE),
    m_param_mod (MOD_FM),
    m_param_freq (DEFAULT_FREQ),
    m_param_ampl (DEFAULT_AMPL),
    m_restart (false)
{    
    add_param ("wave", node_param::INT, &m_param_wave);
    add_param ("modulator", node_param::INT, &m_param_mod);
    add_param ("frequency", node_param::FLOAT, &m_param_freq);
    add_param ("amplitude", node_param::FLOAT, &m_param_ampl);

    update_osc_params();
}

node_oscillator::~node_oscillator()
{
}

void node_oscillator::update_osc_params ()
{
    m_oscillator.set_frequency (m_param_freq);
    m_oscillator.set_amplitude (m_param_ampl);
    m_oscillator.set_wave ((oscillator::wave_type) m_param_wave);
    m_oscillator.set_modulator ((oscillator::mod_type) m_param_mod);
}

void node_oscillator::update_osc (sample* out)
{
    const sample_buffer* pitch_buf = get_input<sample_buffer> (LINK_CONTROL, IN_C_FREQUENCY);
    const sample_buffer* trig_buf = get_input<sample_buffer> (LINK_CONTROL, IN_C_TRIGGER);
    envelope_simple mod_env = get_in_envelope (LINK_CONTROL, IN_C_FREQUENCY);
    envelope_simple trig_env =  get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);
    
    const sample* mod = pitch_buf ? pitch_buf->get_data() : NULL;

    update_osc_params ();

    /* Fill the output. */
    size_t start = 0;
    size_t end = get_info ().block_size;

    while (start < get_info ().block_size) {
	if (m_restart) {
	    if (trig_buf && trig_buf->get_data () [start] != 0.0f)
		m_oscillator.restart ();
	    m_restart = false;
	}
	
	if (trig_buf)
	    end = trig_buf->find_hill (start);
	
	if (mod) {
	    m_oscillator.update (out + start, mod, mod_env, end - start);
	} else
	    m_oscillator.update (out + start, end - start);

	float env_val = trig_env.update(end - start);
	if (env_val == 1.0f && trig_buf && trig_buf->get_data () [end - 1] == 0.0f)
	    m_restart = true;
	    
	start = end;
    }

    /* Modulate amplitude with trigger envelope. */
    if (trig_buf) {
	int n_samp = get_info().block_size;
	const sample* trig = trig_buf->get_data ();
	trig_env = get_in_envelope(LINK_CONTROL, IN_C_TRIGGER);
	while (n_samp--) {
	    float env_val = trig_env.update();
	    *out = *out * ((1.0f - env_val) + (env_val * *trig));
	    ++out;
	    ++trig;
	}
    }
}

} /* namespace psynth */
