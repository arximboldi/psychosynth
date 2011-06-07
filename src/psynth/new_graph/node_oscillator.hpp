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

#ifndef PSYNTH_OBJECTOSCILLATOR_H
#define PSYNTH_OBJECTOSCILLATOR_H

#include <iostream>
#include <cmath>

#include <boost/variant.hpp>
#include <psynth/graph/node.hpp>
#include <psynth/graph/node_factory.hpp>
#include <psynth/graph/node_types.hpp>
#include <psynth/synth/oscillator.hpp>

namespace psynth
{
namespace graph
{

class node_oscillator : public node
{		
public:	
    enum in_audio_socket_id {
	N_IN_A_SOCKETS
    };
	
    enum in_control_socket_id {
	IN_C_FREQUENCY,
	IN_C_AMPLITUDE,
	IN_C_TRIGGER,
	N_IN_C_SOCKETS
    };

    enum wave_type {
	OSC_SINE,
	OSC_SQUARE,
	OSC_TRIANGLE,
	OSC_SAWTOOTH,
	OSC_MOOGSAW,
	OSC_EXP,
	N_OSC_TYPES
    };

    enum mod_type {
	MOD_FM = 0,
	MOD_AM,
	MOD_PM,
	N_MOD_TYPES
    };
    
    enum param_id {
	PARAM_WAVE = node::N_COMMON_PARAMS,
	PARAM_MODULATOR,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_FREQ = 110.0f;
    static const float DEFAULT_AMPL = 0.4f;
    
protected:
    typedef boost::variant<synth::oscillator<synth::sine_generator>,
                           synth::oscillator<synth::square_generator>,
                           synth::oscillator<synth::triangle_generator>,
                           synth::oscillator<synth::sawtooth_generator>,
                           synth::oscillator<synth::moogsaw_generator>,
                           synth::oscillator<synth::exp_generator>
                           > any_oscillator;
    
    any_oscillator m_oscillator;

    void update_osc_params ();
    void update_osc (sample* out);

    // FIXME: THIS IS DIRTY HACK 
public:
    int   m_param_wave;
    int   m_param_wave_new;
    int   m_param_mod;
    float m_param_freq;
    float m_param_ampl;
    bool  m_restart;
    
public:
    node_oscillator (const audio_info& prop,
		     int obj_type,
		     const std::string& name,
		     int n_audio_out,
		     int n_control_out);
    
    ~node_oscillator ();
};

class node_audio_oscillator : public node_oscillator
{
public:
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

private:
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:
    node_audio_oscillator (const audio_info& prop) :
	node_oscillator (prop,
			 NODE_OSCILLATOR,
			 "oscillator",
			 N_OUT_A_SOCKETS,
			 0)
	{};
};

class node_lfo : public node_oscillator
{
public:	
    enum out_control_socket_id {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };

private:
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:
    node_lfo(const audio_info& prop) :
	node_oscillator(prop,
			 NODE_LFO,
			 "lfo",
			 0,
			 N_OUT_C_SOCKETS)
	{};
};

PSYNTH_DECLARE_NODE_FACTORY (node_lfo, "lfo");
PSYNTH_DECLARE_NODE_FACTORY (node_audio_oscillator, "oscillator");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTOSCILLATOR_H */
