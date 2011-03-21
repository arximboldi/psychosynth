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
#include "graph/node_types.hpp"
#include "graph/node_mixer.hpp"

using namespace std;

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_NODE_FACTORY (node_audio_mixer);
PSYNTH_DEFINE_NODE_FACTORY (node_control_mixer);

void node_control_mixer::do_update (const node* caller,
				    int caller_port_type, int caller_port)
{
    sample_buffer* buf = get_output<sample_buffer>(LINK_CONTROL, OUT_C_OUTPUT);
    const sample_buffer* in  = NULL;
    size_t j;
    bool input = false;
    
    init ((sample*) &range (*buf)[0], get_info().block_size);
    
    for (j = 0; j < m_numchan; ++j)
	if ((in = get_input <sample_buffer> (LINK_CONTROL, j))) {
	    link_envelope env = get_in_envelope (LINK_CONTROL, j);
	    mix ((sample*) &range (*buf)[0],
                 (const sample*) &const_range (*in)[0],
                 env, get_info().block_size);
	    input = true;
	}

    if (!input)
	memset ((sample*) &range (*buf)[0],
                0, sizeof(sample) * get_info().block_size);

}

void node_audio_mixer::do_update (const node* caller,
				  int caller_port_type,
				  int caller_port)
{
    audio_buffer* buf = get_output<audio_buffer> (LINK_AUDIO, OUT_A_OUTPUT);
    const audio_buffer* in = NULL;
    const sample_buffer * ampl = get_input<sample_buffer> (LINK_CONTROL, IN_C_AMPLITUDE);
    size_t i, j;
    bool input = false;
    
    for (i = 0; i < get_info().num_channels; ++i)
    {
	init ((sample*) &range (*buf) [0][i],
              get_info().block_size);

	for (j = 0; j < m_numchan; ++j)
	    if ((in = get_input <audio_buffer> (LINK_AUDIO, j))) {
		link_envelope env = get_in_envelope(LINK_AUDIO, j);

		if (!ampl)
		    mix((sample*) &range (*buf) [0][i],
                        (const sample*) &const_range (*in) [0][i],
			env, get_info().block_size);
		else {
		    link_envelope ctrl_env = get_in_envelope(LINK_CONTROL,
                                                             IN_C_AMPLITUDE);
		    mix((sample*) &range (*buf) [0][i],
                        (const sample*) &const_range (*in) [0][i],
                        (const sample*) &const_range (*ampl) [0][i],
			env, ctrl_env, get_info().block_size);
		}
		input = true;
	    }

	if (!input)
	    memset((sample*) &range (*buf) [0][i],
                   0,
		   sizeof(sample) * get_info ().block_size);
    }
}

node_mixer::node_mixer (const audio_info& info,
			int node_type,
			const std::string& name,
			int num_audio_out,
			int num_ctrl_out,
			int num_in): 
    node (info,
	  node_type,
	  name,
	  num_audio_out ? num_in : 0, 
	  num_ctrl_out  ? num_in : N_IN_C_SOCKETS,
	  num_audio_out,
	  num_ctrl_out),
    m_numchan(num_in),
    m_param_ampl(1.0f),
    m_param_mixop(MIX_SUM)
{
    add_param ("amplitude", node_param::FLOAT, &m_param_ampl);
    add_param ("mixop", node_param::INT, &m_param_mixop);
}

void node_mixer::mix (sample* dest, const sample* src, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while (n_samples--)
	    *dest++ += *src++ * m_param_ampl;
    else if (m_param_mixop == MIX_PRODUCT)
	while (n_samples--)
	    *dest++ *= *src++ * m_param_ampl;
}
  
void node_mixer::mix (sample* dest, const sample* src,
		      const sample* ampl, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while (n_samples--)
	    *dest++ += *src++ * (m_param_ampl + m_param_ampl * *ampl++);
    else if (m_param_mixop == MIX_PRODUCT)
	while (n_samples--)
	    *dest++ *= *src++ * (m_param_ampl + m_param_ampl * *ampl++);
}

void node_mixer::mix (sample* dest, const sample* src,
		      link_envelope& env, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while (n_samples--)
	    *dest++ += *src++ * m_param_ampl *
                (float) (link_envelope::sample_type) env.update();
    else if (m_param_mixop == MIX_PRODUCT)
	while (n_samples--)
	    *dest++ *= *src++ * m_param_ampl *
                (float) (link_envelope::sample_type) env.update();
}
  
void node_mixer::mix (sample* dest, const sample* src,
		      const sample* ampl,
		      link_envelope& env,
		      link_envelope& ctrl_env,
		      size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while (n_samples--)
	    *dest++ +=
		*src++ * (m_param_ampl + m_param_ampl * *ampl++ *
                          (float) (link_envelope::sample_type) ctrl_env.update()) *
                (float) (link_envelope::sample_type) env.update();
    else if (m_param_mixop == MIX_PRODUCT)
	while (n_samples--)
	    *dest++ *=
		*src++ * (m_param_ampl + m_param_ampl * *ampl++ *
                          (float) (link_envelope::sample_type) ctrl_env.update()) *
                (float) (link_envelope::sample_type) env.update();
}

void node_mixer::init (sample* dest, size_t n_samples)
{
    float def_val = (m_param_mixop == MIX_SUM ? 0.0 : 1.0);
    std::fill (dest, dest + n_samples, def_val);
}

} /* namespace graph */
} /* namespace psynth */
