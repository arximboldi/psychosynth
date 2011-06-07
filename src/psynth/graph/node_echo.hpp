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

#ifndef PSYNTH_OBJECTECHO_H
#define PSYNTH_OBJECTECHO_H

#include <vector>

#include <psynth/graph/node.hpp>
#include <psynth/graph/node_factory.hpp>

namespace psynth
{
namespace graph
{

class node_echo : public node
{
public:	
    enum in_audio_socket_id {
	IN_A_INPUT,
	N_IN_A_SOCKETS
    };
	
    enum in_control_socket_id {
	IN_C_DELAY,
	IN_C_FEEDBACK,
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
	PARAM_FEEDBACK,
	PARAM_HIDAMP,
	N_PARAM
    };

    static constexpr float MAX_DELAY = 1.0f;
    static constexpr float DEFAULT_DELAY = 0.5f;
    static constexpr float DEFAULT_FEEDBACK = 0.5f;
    static constexpr float DEFAULT_HIDAMP = 0.5f;
    
private:
    float m_param_delay;
    float m_old_param_delay;
    float m_param_feedback;
    float m_param_hidamp;
    int m_pos;

    std::vector<sample> m_old_val;
    
    audio_buffer m_buffer;

    int do_update_channel (int chan);
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void on_info_change ();
    void do_advance () {}
    
public:
    node_echo(const audio_info& prop);
    ~node_echo();
};

PSYNTH_DECLARE_NODE_FACTORY(node_echo, "echo");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTECHO_H */
