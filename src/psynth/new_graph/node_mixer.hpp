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

#ifndef PSYNTH_OBJECTMIXER_H
#define PSYNTH_OBJECTMIXER_H

#include <psynth/graph/node_types.hpp>
#include <psynth/graph/node.hpp>
#include <psynth/graph/node_factory.hpp>

namespace psynth
{
namespace graph
{

class node_mixer : public node
{
public:
       /*enum {
      N_IN_A_SOCKETS
      };*/
	
    enum in_control_socket_id {
	IN_C_AMPLITUDE,
	N_IN_C_SOCKETS
    };

    enum param_id {
	PARAM_AMPLITUDE = node::N_COMMON_PARAMS,
	PARAM_MIXOP,
	N_PARAM
    };

    enum mix_op {
	MIX_SUM,
	MIX_PRODUCT,
	N_MIXOPS
    };


protected:
    size_t m_numchan;

    void mix (sample* dest, const sample* src, size_t n_samples);
    
    void mix (sample* dest, const sample* src,
	     const sample* ampl, size_t n_samples);

    void mix (sample* dest, const sample* src,
	     link_envelope& env, size_t n_samples);

    void mix (sample* dest, const sample* src, const sample* ampl,
              link_envelope& env, link_envelope& ctrl_env,
              size_t n_samples);
    
    void init (sample* dest, size_t n_samples);
    
private:
    float m_param_ampl;
    int m_param_mixop;    
    
public:
    node_mixer (const audio_info& info,
		int obj_type,
		const std::string& name,
		int num_audio_out,
		int num_ctrl_out,
		int num_in = 2);
};

class node_audio_mixer : public node_mixer
{
public:
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

    enum out_control_socket_id {
	N_OUT_C_SOCKETS
    };
    
private:
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:  
    node_audio_mixer (const audio_info& prop, int n_chan = 2) :
	node_mixer (prop,
		    NODE_MIXER,
		    "audio_mixer",
		    N_OUT_A_SOCKETS,
		    N_OUT_C_SOCKETS,
		    n_chan)
	{}
};

class node_control_mixer : public graph::node_mixer
{
public:
    enum out_audio_socket_id {
	N_OUT_A_SOCKETS
    };

    enum out_control_socket_id {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };
    
private:
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:  
    node_control_mixer (const audio_info& prop, int n_chan = 2) :
	node_mixer (prop,
		    NODE_CONTROLMIXER,
		    "control_mixer",
		    N_OUT_A_SOCKETS,
		    N_OUT_C_SOCKETS,
		    n_chan)
	{}
};

PSYNTH_DECLARE_NODE_FACTORY (node_control_mixer, "control_mixer");
PSYNTH_DECLARE_NODE_FACTORY (node_audio_mixer, "audio_mixer");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTMIXER_H */
