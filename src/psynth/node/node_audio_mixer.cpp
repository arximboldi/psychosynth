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
#include "node/node_audio_mixer.hpp"

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY (node_audio_mixer);

void node_audio_mixer::do_update (const node* caller,
				  int caller_port_type,
				  int caller_port)
{
    audio_buffer* buf = get_output<audio_buffer> (LINK_AUDIO, OUT_A_OUTPUT);
    const audio_buffer* in = NULL;
    const sample_buffer * ampl = get_input<sample_buffer> (LINK_CONTROL, IN_C_AMPLITUDE);
    size_t i, j;
    bool input = false;
    
    for (i = 0; i < get_info().num_channels; ++i) {
	init(buf->get_channel(i), get_info().block_size);

	for (j = 0; j < m_numchan; ++j)
	    if ((in = get_input <audio_buffer> (LINK_AUDIO, j))) {
		envelope_simple env = get_in_envelope(LINK_AUDIO, j);

		if (!ampl)
		    mix(buf->get_channel(i), in->get_channel(i),
			env, get_info().block_size);
		else {
		    envelope_simple ctrl_env = get_in_envelope(LINK_CONTROL,
							       IN_C_AMPLITUDE);
		    mix(buf->get_channel(i),
			in->get_channel(i),
			ampl->get_data(),
			env, ctrl_env, get_info ().block_size);
		}
		input = true;
	    }

	if (!input)
	    memset(buf->get_channel(i), 0,
		   sizeof(sample) * get_info ().block_size);
    }
}

} /* namespace psynth */
