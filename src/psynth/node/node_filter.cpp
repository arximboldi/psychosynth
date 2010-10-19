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
#include "node/node_filter.hpp"
#include "node/node_types.hpp"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY(node_filter);

node_filter::node_filter(const audio_info& prop, int mode) : 
    node (prop,
	  NODE_FILTER,
	  "filter",
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_param_type(mode),
    m_param_cutoff(DEFAULT_CUTOFF),
    m_param_resonance(DEFAULT_RESONANCE),
    m_filter_values((filter_values::type) m_param_type,
		    m_param_cutoff,
		    m_param_resonance,
		    prop.sample_rate),
    m_filter (prop.num_channels, filter (&m_filter_values))
{
    add_param ("type", node_param::INT, &m_param_type);
    add_param ("cutoff", node_param::FLOAT, &m_param_cutoff);
    add_param ("resonance", node_param::FLOAT, &m_param_resonance);

    //for (int i = 0; i < prop.num_channels; ++i)
}

/* TODO cambiar el numero de canales. */

node_filter::~node_filter()
{
}

void node_filter::do_update (const node* caller,
			      int caller_port_type, int caller_por)
{
    const audio_buffer* input = get_input<audio_buffer>(LINK_AUDIO, IN_A_INPUT);
    const sample_buffer* cutoff = get_input<sample_buffer>(LINK_CONTROL, IN_C_CUTOFF);
    audio_buffer* output = get_output<audio_buffer>(LINK_AUDIO, IN_A_INPUT);    

    if (input) {
	if (m_param_type != m_filter_values.get_type() ||
	    m_param_cutoff != m_filter_values.get_frequency() ||
	    m_param_resonance != m_filter_values.get_resonance())
	    m_filter_values.calculate((filter_values::type) m_param_type,
				      m_param_cutoff,
				      m_param_resonance,
				      get_info().sample_rate);
	
	for (size_t i = 0; i < get_info().num_channels; ++i) {
	    sample* outbuf = output->get_channel(i);
	    const sample* inbuf = input->get_channel(i);
	    filter& filter = m_filter[i];
	    envelope_simple env = get_in_envelope (LINK_AUDIO, IN_A_INPUT);
	    if (!cutoff)
		for (size_t j = 0; j < output->size(); ++j)
		    *outbuf++ = filter.update(*inbuf++ * env.update());
	    else {
		envelope_simple mod_env = get_in_envelope (LINK_CONTROL,
							  IN_C_CUTOFF);
		const sample* cutoff_buf = cutoff->get_data();
		
		for (size_t j = 0; j < output->size(); ++j) {
		    /* FIXME: Slow */
		    m_filter_values.calculate(m_param_cutoff
					      + *cutoff_buf++
					      * m_param_cutoff
					      * mod_env.update());
		    *outbuf++ = filter.update(*inbuf++ * env.update());
		}
	    }
	}
    } else {
	output->zero();
    }
}

} /* namespace psynth */
