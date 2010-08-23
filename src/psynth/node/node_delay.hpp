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

#ifndef PSYNTH_OBJECTDELAY_H
#define PSYNTH_OBJECTDELAY_H

#include <vector>

#include <psynth/node/node.hpp>
#include <psynth/node/node_factory.hpp>

namespace psynth
{

class node_delay : public node
{
public:	
    enum in_audio_socket_id {
	IN_A_INPUT,
	N_IN_A_SOCKETS
    };
	
    enum in_control_socket_id {
	IN_C_DELAY,
	IN_C_DEPTH,
	N_IN_C_SOCKETS
    };
	
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };
	
    enum out_control_socket_id {
	N_OUT_C_SOCKETS
    };

    enum param_id {
	PARAM_DELAY = node::N_COMMON_PARAMS,
	PARAM_DEPTH,
	N_PARAM
    };

    static const float MAX_DELAY = 0.1f;
    static const float DEFAULT_DELAY = 0.0015f;
    static const float DEFAULT_DEPTH = 0.5f;
    
private:
    float m_param_delay;
    float m_param_depth;
    int m_max_delay_pos;
    int m_pos;
    
    audio_buffer m_buffer;

    int do_update_channel (int chan);
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void on_info_change ();
    void do_advance () {}
    
public:
    node_delay (const audio_info& prop);
    ~node_delay ();
};

PSYNTH_DECLARE_NODE_FACTORY (node_delay, "delay");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTDELAY_H */

