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

#ifndef PSYNTH_OBJECTFILTER_H
#define PSYNTH_OBJECTFILTER_H

#include <vector>

#include <psynth/graph/node.hpp>
#include <psynth/graph/node_factory.hpp>
#include <psynth/synth/filter.hpp>

namespace psynth
{
namespace graph
{

class node_filter : public node
{
public:	
    enum in_audio_socket_id {
	IN_A_INPUT,
	N_IN_A_SOCKETS
    };
	
    enum in_control_socket_id {
	IN_C_CUTOFF,
	IN_C_EMPHASIS,
	N_IN_C_SOCKETS
    };
	
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };
	
    enum out_control_socket_id {
	N_OUT_C_SOCKETS
    };

    enum filter_type {
	FILTER_LOWPASS       = filter_values::LOWPASS,
	FILTER_HIGHPASS      = filter_values::HIPASS,
	FILTER_BANDPASS_CSG  = filter_values::BANDPASS_CSG,
	FILTER_BANDPASS_CZPG = filter_values::BANDPASS_CZPG,
	FILTER_NOTCH         = filter_values::NOTCH,
	FILTER_MOOG          = filter_values::MOOG,
	N_FILTER_TYPES,
    };

    enum param_id {
	PARAM_TYPE = node::N_COMMON_PARAMS,
	PARAM_CUTOFF,
	PARAM_RESONANCE,
	N_PARAM
    };

    static constexpr float DEFAULT_CUTOFF    = 660.0f;
    static constexpr float DEFAULT_RESONANCE = 0.5f;
    
private:
    int m_param_type;
    float m_param_cutoff;
    float m_param_resonance;

    filter_values m_filter_values;
    std::vector<filter> m_filter;
    
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:
    node_filter (const audio_info& prop, int mode = FILTER_LOWPASS);
    ~node_filter ();
};

PSYNTH_DECLARE_NODE_FACTORY(node_filter, "filter");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTFILTER_H */
