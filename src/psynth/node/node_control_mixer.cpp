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
#include "node/node_control_mixer.hpp"

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY(node_control_mixer);

void node_control_mixer::do_update (const node* caller,
				    int caller_port_type, int caller_port)
{
    sample_buffer* buf = get_output<sample_buffer >(LINK_CONTROL, OUT_C_OUTPUT);
    const sample_buffer* in = NULL;
    size_t j;
    bool input = false;
    
    init (buf->get_data(), get_info().block_size);
    
    for (j = 0; j < m_numchan; ++j)
	if ((in = get_input <sample_buffer> (LINK_CONTROL, j))) {
	    envelope_simple env = get_in_envelope (LINK_CONTROL, j);
	    mix(buf->get_data(), in->get_data(), env, get_info().block_size);
	    input = true;
	}

    if (!input)
	memset(buf->get_data(), 0, sizeof(sample) * get_info().block_size);

}

} /* namespace psynth */
