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

#ifndef PSYNTH_OBJECTNOISE_H
#define PSYNTH_OBJECTNOISE_H

#include <psynth/graph/node.hpp>
#include <psynth/graph/node_types.hpp>
#include <psynth/graph/node_factory.hpp>

namespace psynth
{
namespace graph
{

class node_noise : public node0
{
public:
    enum in_audio_socket_id {
	N_IN_A_SOCKETS
    };

    enum in_control_socket_id {
	IN_C_AMPLITUDE,
	IN_C_TRIGGER,
	N_IN_C_SOCKETS
    };

    enum type {
	NOISE_WHITE,
	NOISE_PINK,
	N_TYPES
    };

    enum param_id {
	PARAM_TYPE = node0::N_COMMON_PARAMS,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static constexpr float DEFAULT_AMPL = 0.3f;

protected:
    sample update_pink ();
    sample update_white ();
    void update_noise (sample* buf);

private:
    int   m_param_type;
    float m_param_ampl;

    /* Pink noise factors. */
    sample m_b0;
    sample m_b1;
    sample m_b2;
    sample m_b3;
    sample m_b4;
    sample m_b5;
    sample m_b6;

    //bool  m_restart;

public:
    node_noise (const audio_info& prop,
		int obj_type,
		const std::string& name,
		int n_audio_out,
		int n_control_out);

    ~node_noise ();
};

class node_audio_noise : public node_noise
{
public:
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

private:
    void do_update (const node0* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}

public:
    node_audio_noise (const audio_info& prop) :
	node_noise (prop,
		   NODE_AUDIONOISE,
		   "audio_noise",
		   N_OUT_A_SOCKETS,
		   0)
	{};
};

class node_control_noise : public node_noise
{
public:
    enum out_control_socket_id {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };

private:
    void do_update (const node0* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}

public:
    node_control_noise (const audio_info& prop) :
	node_noise (prop,
		    NODE_CONTROLNOISE,
		    "control_noise",
		    0,
		    N_OUT_C_SOCKETS)
	{};
};

PSYNTH_DECLARE_NODE_FACTORY (node_control_noise, "control_noise");
PSYNTH_DECLARE_NODE_FACTORY (node_audio_noise,   "audio_noise");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTNOISE_H */
