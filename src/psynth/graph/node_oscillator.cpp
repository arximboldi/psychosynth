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

#include "synth/util.hpp"
#include "graph/node_types.hpp"
#include "graph/node_oscillator.hpp"

using namespace std;

namespace psynth
{
namespace graph
{

namespace
{

struct do_update_osc : public boost::static_visitor<void>
{
    const sample_range& _data;
    
    do_update_osc (const sample_range& data)
        : _data (data) {}
    
    template <class Osc>
    void operator () (Osc& osc) const
    {
        osc.update (_data);
    }
};


/**
 * @todo ADD THE MODULATION ENVELOPE!
 */
struct do_update_osc_mod : public boost::static_visitor<void>
{
    int _mod_type;
    node::link_envelope& _mod_env;
    const sample_const_range& _mod;
    const sample_range& _data;
    
    do_update_osc_mod (int mod_type,
                       node::link_envelope& mod_env,
                       const sample_const_range& mod, 
                       const sample_range& data)
        : _mod_type (mod_type)
        , _mod_env (mod_env)
        , _mod (mod)
        , _data (data)
    {}
    
    template <class Osc>
    void operator () (Osc& osc) const
    {
        switch (_mod_type)
        {
        case node_oscillator::MOD_FM:
            osc.update_fm (_data, _mod);
            break;
        case node_oscillator::MOD_AM:
            osc.update_am (_data, _mod);
            break;            
	case node_oscillator::MOD_PM:
            osc.update_pm (_data, _mod);
            break;
        default:
            assert (false);
        }
    }
};

struct do_update_osc_params : public boost::static_visitor<void>
{
    node_oscillator* _this;
    do_update_osc_params (node_oscillator* t) : _this (t) {}

    template <class Osc>
    void operator () (Osc& osc) const
    {
        osc.set_frequency (_this->m_param_freq);
        osc.set_amplitude (_this->m_param_ampl);
    }
};

struct do_restart : public boost::static_visitor<void>
{
    template <class Osc> void operator () (Osc& osc) const
    { osc.restart (); }
};

} /* anonymous namespace */

PSYNTH_DEFINE_NODE_FACTORY (node_lfo);
PSYNTH_DEFINE_NODE_FACTORY (node_audio_oscillator);

void node_audio_oscillator::do_update (
    const node* caller, int caller_port_type, int caller_port)
{
    audio_buffer* buf = get_output<audio_buffer> (LINK_AUDIO, OUT_A_OUTPUT);
    sample* out = (sample*) &range (*buf) [0][0];

    update_osc (out);
	
    /* Copy on the other channels. */
    for (size_t i = 1; i < (size_t) get_info().num_channels; i++)
	memcpy(&range (*buf)[0][i], &range (*buf)[0][0],
               sizeof(sample) * get_info().block_size);
}

void node_lfo::do_update (const node* caller,
			   int caller_port_type, int caller_port)
{
    sample_buffer* buf = get_output <sample_buffer>(LINK_CONTROL, OUT_C_OUTPUT);
    sample* out = (sample*) &range (*buf) [0][0]; //buf->get_data ();

    update_osc (out);
}

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
    m_oscillator (synth::oscillator<> (prop.sample_rate)),
    m_param_wave (OSC_SINE),
    m_param_wave_new (N_OSC_TYPES),
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
    if (m_param_wave != m_param_wave_new)
    {
        switch (m_param_wave)
        {
        case OSC_SINE: m_oscillator = synth::oscillator<synth::sine_generator> (get_info ().sample_rate); break;
        case OSC_SQUARE: m_oscillator = synth::oscillator<synth::square_generator> (get_info ().sample_rate); break;
        case OSC_TRIANGLE: m_oscillator = synth::oscillator<synth::triangle_generator> (get_info ().sample_rate); break;
        case OSC_SAWTOOTH: m_oscillator = synth::oscillator<synth::sawtooth_generator> (get_info ().sample_rate); break;
        case OSC_MOOGSAW: m_oscillator = synth::oscillator<synth::moogsaw_generator> (get_info ().sample_rate); break;
        case OSC_EXP: m_oscillator = synth::oscillator<synth::exp_generator> (get_info ().sample_rate); break;
        default: assert (false);
        }
 
        m_param_wave_new = m_param_wave;
    }
    
    boost::apply_visitor (do_update_osc_params (this), m_oscillator);
}

void node_oscillator::update_osc (sample* out)
{
    const sample_buffer* pitch_buf = get_input<sample_buffer> (LINK_CONTROL, IN_C_FREQUENCY);
    const sample_buffer* trig_buf = get_input<sample_buffer> (LINK_CONTROL, IN_C_TRIGGER);
    link_envelope mod_env = get_in_envelope (LINK_CONTROL, IN_C_FREQUENCY);
    link_envelope trig_env =  get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);
    
    const sample* mod = pitch_buf ? (const sample*) &const_range (*pitch_buf) [0] : 0;
    
    update_osc_params ();

    /* Fill the output. */
    size_t start = 0;
    size_t end = get_info ().block_size;

    while (start < get_info ().block_size) {
	if (m_restart) {
	    if (trig_buf && const_range (*trig_buf) [start] != 0.0f)
                boost::apply_visitor (do_restart (), m_oscillator);
	    m_restart = false;
	}
	
	if (trig_buf)
	    end = synth::find_hill (const_range (*trig_buf), start);
	
	if (mod)
        {
            boost::apply_visitor (
                do_update_osc_mod (m_param_mod,
                                   mod_env,
                                   sample_const_range (end-start, (const sample_frame*) mod + start),
                                   sample_range (end-start, (sample_frame*) out + start)),
                m_oscillator);
	}
        else
        {
            boost::apply_visitor (
                do_update_osc (sample_range (end - start, (sample_frame*) out + start)),
                m_oscillator);
        }
        
	float env_val = (float) (audio_sample) trig_env.update (end - start);
	if (env_val == 1.0f && trig_buf && const_range (*trig_buf) [end - 1] == 0.0f)
	    m_restart = true;
	    
	start = end;
    }

    /* Modulate amplitude with trigger envelope. */
    if (trig_buf)
    {
	int n_samp = get_info().block_size;
	const sample* trig = (const sample*) &const_range (*trig_buf) [0];
	trig_env = get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);
	while (n_samp--) {
	    auto env_val = (float) (audio_sample) trig_env.update();
	    *out = *out * ((1.0f - env_val) + (env_val * *trig));
	    ++out;
	    ++trig;
	}
    }
}

} /* namespace graph */
} /* namespace psynth */
